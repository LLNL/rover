#include <utils/rover_logging.hpp>
#include <compositing/compositor.hpp>
#include <compositing/volume_block.hpp>
#include <algorithm>
#include <assert.h>
#include <limits>
#ifdef PARALLEL
#include <compositing/volume_redistribute.hpp>
#endif

namespace rover {

VolumeCompositor::VolumeCompositor()
{

}

VolumeCompositor::~VolumeCompositor()
{

}

template<typename FloatType> 
PartialImage<FloatType> 
VolumeCompositor::composite(std::vector<PartialImage<FloatType>> &partial_images)
{
  int total_partial_comps = 0;
  const int num_partial_images = static_cast<int>(partial_images.size());
  int *offsets = new int[num_partial_images];
  int *pixel_mins =  new int[num_partial_images];
  int *pixel_maxs =  new int[num_partial_images];

  for(int i = 0; i < num_partial_images; ++i)
  {
    assert(partial_images[i].m_buffer.GetNumChannels() == 4);
    offsets[i] = total_partial_comps;
    total_partial_comps += partial_images[i].m_buffer.GetSize();
    ROVER_INFO("Domain : "<<i<<" with "<<partial_images[i].m_buffer.GetSize());
  }

  ROVER_INFO("Total number of partial composites "<<total_partial_comps);

  std::vector<VolumePartial> partials;
  partials.resize(total_partial_comps);
  for(int i = 0; i < num_partial_images; ++i)
  {
    const int image_size = partial_images[i].m_buffer.GetSize();
    #pragma omp parallel for
    for(int j = 0; j < image_size; ++j)
    {
      int index = offsets[i] + j;
      partials[index].m_pixel_id = static_cast<int>(partial_images[i].m_pixel_ids.GetPortalConstControl().Get(j));
      partials[index].m_depth = static_cast<float>(partial_images[i].m_distances.GetPortalConstControl().Get(j));
      partials[index].m_pixel[0] = static_cast<unsigned char>(partial_images[i].
                                  m_buffer.Buffer.GetPortalConstControl().Get(j*4+0) * 255);

      partials[index].m_pixel[1] = static_cast<unsigned char>(partial_images[i].
                                  m_buffer.Buffer.GetPortalConstControl().Get(j*4+1) * 255);

      partials[index].m_pixel[2] = static_cast<unsigned char>(partial_images[i].
                                  m_buffer.Buffer.GetPortalConstControl().Get(j*4+2) * 255);

      partials[index].m_alpha = static_cast<float>(partial_images[i].
                                  m_buffer.Buffer.GetPortalConstControl().Get(j*4+3));

    }

    //calculate the range of pixel ids each domain has
    int max_pixel = std::numeric_limits<int>::min();
    #pragma omp parallel for reduction(max:max_pixel)
    for(int j = 0; j < image_size; ++j)
    {
      int val = static_cast<int>(partial_images[i].m_pixel_ids.GetPortalConstControl().Get(j));
      if(val > max_pixel)
      {
        max_pixel = val;
      }
    }
    int min_pixel = std::numeric_limits<int>::max();
    #pragma omp parallel for reduction(max:min_pixel)
    for(int j = 0; j < image_size; ++j)
    {
      
      int val = static_cast<int>(partial_images[i].m_pixel_ids.GetPortalConstControl().Get(j));
      if(val < min_pixel)
      {
        min_pixel = val;
      }

      assert(min_pixel > -1);
      pixel_mins[i] = min_pixel;
      pixel_maxs[i] = max_pixel;
    }

  }// for each partial image
  
  // 
  // determine the global pixel mins and maxs
  //
  int global_min_pixel = std::numeric_limits<int>::max();
  int global_max_pixel = std::numeric_limits<int>::min();
  for(int i = 0; i < num_partial_images; ++i)
  {
    global_min_pixel = std::min(global_min_pixel, pixel_mins[i]);   
    global_max_pixel = std::max(global_max_pixel, pixel_maxs[i]);   
  }

#ifdef PARALLEL
  int rank_min = global_min_pixel;
  int rank_max = global_max_pixel;
  int mpi_min;
  int mpi_max;
  MPI_Allreduce(&rank_min, &mpi_min, 1, MPI_INT, MPI_MIN, m_comm_handle);
  MPI_Allreduce(&rank_max, &mpi_max, 1, MPI_INT, MPI_MAX, m_comm_handle);
  global_min_pixel = mpi_min;
  global_max_pixel = mpi_max;
#endif

  delete[] offsets;
  delete[] pixel_mins;
  delete[] pixel_maxs;

  ROVER_INFO("Extacted partial structs");

  //
  // TODO: check to see if we have less than one
  //
  assert(total_partial_comps > 1);

  //
  // Sort the composites
  //
  std::sort(partials.begin(), partials.end());  
  ROVER_INFO("Sorted partials");
  // 
  // Find the number of unique pixel_ids with work
  //
  std::vector<unsigned char> work_flags;
  std::vector<unsigned char> unique_flags;
  work_flags.resize(total_partial_comps);
  unique_flags.resize(total_partial_comps);
  //
  // just check the first and last entries manualy to reduce the
  // loop complexity
  //
  if(partials[0].m_pixel_id == partials[1].m_pixel_id)
  {
    work_flags[0] = 1;
    unique_flags[0] = 0;
  }
  else
  {
    work_flags[0] = 0;
    unique_flags[0] = 1;
  }
  if(partials[total_partial_comps].m_pixel_id != partials[total_partial_comps -1].m_pixel_id)
  {
    unique_flags[total_partial_comps] = 1;
  }
  else
  {
    unique_flags[total_partial_comps] = 0;
  }
  const int n_minus_one =  total_partial_comps - 1;

  #pragma omp parallel for
  for(int i = 1; i < n_minus_one; ++i)
  {
    unsigned char work_flag = 0;
    unsigned char unique_flag = 0;
    bool is_begining = false;
    if(partials[i].m_pixel_id != partials[i-1].m_pixel_id)
    {
      is_begining = true;
    }

    bool has_compositing_work = false;
    if(partials[i].m_pixel_id == partials[i+1].m_pixel_id)
    {
      has_compositing_work = true;
    }
    if(is_begining && has_compositing_work)
    {
      work_flag  = 1;
    }
    if(is_begining && !has_compositing_work)
    {
      unique_flag = 1;
    }
    work_flags[i]  = work_flag;
    unique_flags[i] = unique_flag;
  }
  // count the number of of unique pixels
  int total_segments = 0;
  #pragma omp parallel for shared(total_segments, work_flags) reduction(+:total_segments)
  for(int i = 0; i < total_partial_comps; ++i)
  {
    total_segments += work_flags[i];
  }

  int total_unique_pixels = 0;
  #pragma omp parallel for shared(total_unique_pixels, unique_flags) reduction(+:total_unique_pixels)
  for(int i = 0; i < total_partial_comps; ++i)
  {
    total_unique_pixels += unique_flags[i];
  }

  ROVER_INFO("Total pixels that need compositing "<<total_segments<<" total partials "<<total_partial_comps);
  ROVER_INFO("Total unique pixels "<<total_unique_pixels);

  if(total_segments ==  0)
  {
    //nothing to do
    std::cout<<"*****Nothing to do\n";
  }
 
  //
  // find the pixel indexes that have compositing work
  //
  std::vector<int> pixel_work_ids;
  pixel_work_ids.resize(total_segments);
  int current_index = 0;
  for(int i = 0;  i < total_partial_comps; ++i)
  {
    if(work_flags[i] == 1)
    {
      pixel_work_ids[current_index] = i;
      ++current_index;
    }
  }

  //
  // find the pixel indexes that have NO compositing work
  //
  std::vector<int> unique_ids;
  unique_ids.resize(total_unique_pixels);
  current_index = 0;
  for(int i = 0;  i < total_partial_comps; ++i)
  {
    if(unique_flags[i] == 1)
    {
      unique_ids[current_index] = i;
      ++current_index;
    }
  }


  const int total_output_pixels = total_unique_pixels + total_segments;
  ROVER_INFO("Total output size "<<total_output_pixels);
  std::vector<VolumePartial> output_partials;
  output_partials.resize(total_output_pixels);
  VolumePartial bg_color;
  bg_color.m_pixel[0] = 255;
  bg_color.m_pixel[1] = 255;
  bg_color.m_pixel[2] = 255;
  bg_color.m_alpha = 1.f;
  //
  // Gather the unique pixels into the output
  //
  #pragma omp parallel for 
  for(int i = 0; i < total_unique_pixels; ++i)
  {
    VolumePartial result = partials[unique_ids[i]];
    result.blend(bg_color);
    output_partials[i] = result;
  }


  //
  // Perform the compositing and output the result in the output 
  //
  #pragma omp parallel for
  for(int i = 0; i < total_segments; ++i)
  {
    int current_index = pixel_work_ids[i];
    VolumePartial result = partials[current_index];
    //std::cout<<"Compositing "<<result.m_pixel_id<<" ";
    ++current_index;
    VolumePartial next = partials[current_index];
    //std::cout<<"("<<(int)result.m_pixel[0]<<" "<<(int)result.m_pixel[1]<<" " <<(int)result.m_pixel[2]<< " " <<result.m_alpha<<") "; 
    // TODO: we could just count the amount of work and make this a for loop(vectorize??)
    while(result.m_pixel_id == next.m_pixel_id)
    {
      //std::cout<<next.m_pixel_id<<" "<<next.m_depth<<" ";
      result.blend(next);
 //     std::cout<<"("<<(int)result.m_pixel[0]<<" "<<(int)result.m_pixel[1]<<" " <<(int)result.m_pixel[2]<< " " <<result.m_alpha<<") "; 
      if(current_index + 1 >= total_partial_comps || result.m_alpha >= 1.f) 
      {
        break;
      }
      ++current_index;
      next = partials[current_index];
    }
//    std::cout<<"\n";
    if(result.m_alpha < 1.f) result.blend(bg_color);
    result.m_alpha = fmaxf(1.f, result.m_alpha);
    //std::cout<<" Color ("<<(int)result.m_pixel[0]<<" "<<(int)result.m_pixel[1]<<" " <<(int)result.m_pixel[2]<< " " <<(int)result.m_alpha<<")"; 
    if(result.m_pixel_id == 157174)
    {

      result.m_pixel[0] = 255; 
      result.m_pixel[1] = 0; 
      result.m_pixel[2] = 0; 

      result.m_alpha = 1;
    }
    output_partials[total_unique_pixels + i] = result;
  }



  //
  // pack the output back into a channel buffer
  //
  PartialImage<FloatType> output;
  output.m_width = partial_images[0].m_width;
  output.m_height= partial_images[0].m_height;
  output.m_pixel_ids.Allocate(total_output_pixels);
  output.m_distances.Allocate(total_output_pixels);
  //default num channels is 4
  output.m_buffer.Resize(total_output_pixels);
  const FloatType inverse = 1.f / 255.f;
  #pragma omp parallel for
  for(int i = 0; i < total_output_pixels; ++i)
  {
    output.m_pixel_ids.GetPortalControl().Set(i, output_partials[i].m_pixel_id ); 
    output.m_distances.GetPortalControl().Set(i, output_partials[i].m_depth ); 
    const int starting_index = i * 4;
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 0, static_cast<FloatType>(output_partials[i].m_pixel[0])*inverse);
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 1, static_cast<FloatType>(output_partials[i].m_pixel[1])*inverse);
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 2, static_cast<FloatType>(output_partials[i].m_pixel[2])*inverse);
    output.m_buffer.Buffer.GetPortalControl().Set(starting_index + 3, static_cast<FloatType>(output_partials[i].m_alpha));
  }

  return output;


}

#ifdef PARALLEL
void 
VolumeCompositor::set_comm_handle(MPI_Comm comm_handle)
{
  m_comm_handle = comm_handle;
}
#endif

//Explicit function instantiations
template 
PartialImage<vtkm::Float32> 
VolumeCompositor::composite<vtkm::Float32>(std::vector<PartialImage<vtkm::Float32>> &);

template 
PartialImage<vtkm::Float64> 
VolumeCompositor::composite<vtkm::Float64>(std::vector<PartialImage<vtkm::Float64>> &);

} // namespace rover
