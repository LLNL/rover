#include <utils/rover_logging.hpp>
#include <compositing/compositor.hpp>
#include <algorithm>
#include <assert.h>
#include <limits>
#ifdef PARALLEL
#include <compositing/volume_redistribute.hpp>
#endif

namespace rover {
//--------------------------------------------------------------------------------------------
template<typename PartialType>
Compositor<PartialType>::Compositor()
{

}

//--------------------------------------------------------------------------------------------

template<typename PartialType>
Compositor<PartialType>::~Compositor()
{

}

//--------------------------------------------------------------------------------------------

template<typename PartialType>
template<typename FloatType>
void 
Compositor<PartialType>::extract(std::vector<PartialImage<FloatType>> &partial_images, 
                          std::vector<PartialType> &partials,
                          int &global_min_pixel,
                          int &global_max_pixel)
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

  partials.resize(total_partial_comps);
  for(int i = 0; i < num_partial_images; ++i)
  {
    //
    //  Extract the partial composites into a contiguous array
    //

    const int image_size = partial_images[i].m_buffer.GetSize();
    #pragma omp parallel for
    for(int j = 0; j < image_size; ++j)
    {
      int index = offsets[i] + j;
      partials[index].load_from_partial(partial_images[i], j);
    }

    //
    // Calculate the range of pixel ids each domain has
    //
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
  global_min_pixel = std::numeric_limits<int>::max();
  global_max_pixel = std::numeric_limits<int>::min();
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
}

//--------------------------------------------------------------------------------------------

template<typename PartialType>
void 
Compositor<PartialType>::composite_partials(std::vector<PartialType> &partials, 
                                    std::vector<PartialType> &output_partials)
{
  
  const int total_partial_comps = partials.size();
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

  output_partials.resize(total_output_pixels);
  
  // 
  // Gather the unique pixels into the output
  //
  #pragma omp parallel for 
  for(int i = 0; i < total_unique_pixels; ++i)
  {
    VolumePartial result = partials[unique_ids[i]];
    output_partials[i] = result;
  }
 

  //
  // Perform the compositing and output the result in the output 
  //
  #pragma omp parallel for
  for(int i = 0; i < total_segments; ++i)
  {
    int current_index = pixel_work_ids[i];
    PartialType result = partials[current_index];
    ++current_index;
    PartialType next = partials[current_index];
    // TODO: we could just count the amount of work and make this a for loop(vectorize??)
    while(result.m_pixel_id == next.m_pixel_id)
    {
      result.blend(next);
      if(current_index + 1 >= total_partial_comps) 
      {
        // we could break early for volumes,
        // but blending past 1.0 alpha is no op.
        break;
      }
      ++current_index;
      next = partials[current_index];
    }
    output_partials[total_unique_pixels + i] = result;
  }

  //placeholder 
  PartialType::composite_default_background(output_partials);

}

//--------------------------------------------------------------------------------------------

template<typename PartialType>
template<typename FloatType> 
PartialImage<FloatType> 
Compositor<PartialType>::composite(std::vector<PartialImage<FloatType>> &partial_images)
{
  
  std::vector<PartialType> partials;
  int global_min_pixel;
  int global_max_pixel;
  extract(partial_images, partials, global_min_pixel, global_max_pixel);
  
#ifdef PARALLEL
  //
  // Exchange partials with other ranks
  //

  redistribute(partials, 
               m_comm_handle,
               global_min_pixel,
               global_max_pixel);
#endif

  const int  total_partial_comps = partials.size();

  ROVER_INFO("Extacted partial structs");

  //
  // TODO: check to see if we have less than one
  //
  assert(total_partial_comps > 1);
  
  std::vector<PartialType> output_partials;
  composite_partials(partials, output_partials);
   
#ifdef PARALLEL
  //
  // Collect all of the distibuted pixels
  //
  collect(output_partials, m_comm_handle);
#endif
  
  //
  // pack the output back into a channel buffer
  //
  const int num_channels = partial_images[0].m_buffer.GetNumChannels();
  PartialImage<FloatType> output;
  output.m_width = partial_images[0].m_width;
  output.m_height= partial_images[0].m_height;
  const int out_size = output_partials.size();
  output.m_pixel_ids.Allocate(out_size);
  output.m_distances.Allocate(out_size);
  output.m_buffer.SetNumChannels(num_channels);
  output.m_buffer.Resize(out_size);
  #pragma omp parallel for
  for(int i = 0; i < out_size; ++i)
  {
    output_partials[i].store_into_partial(output, i);
  }
  ROVER_INFO("Compositing results in "<<out_size);
  return output;


}

#ifdef PARALLEL
template<typename PartialType>
void 
Compositor<PartialType>::set_comm_handle(MPI_Comm comm_handle)
{
  m_comm_handle = comm_handle;
}
#endif

//Explicit function instantiations
template class Compositor<VolumePartial>;

template 
PartialImage<vtkm::Float32> 
Compositor<VolumePartial>::composite<vtkm::Float32>(std::vector<PartialImage<vtkm::Float32>> &);

template 
PartialImage<vtkm::Float64> 
Compositor<VolumePartial>::composite<vtkm::Float64>(std::vector<PartialImage<vtkm::Float64>> &);
/*
template class Compositor<AbsorptionPartial<vtkm::Float32>>;
template class Compositor<AbsorptionPartial<vtkm::Float64>>;

template 
PartialImage<vtkm::Float32> 
Compositor<AbsorptionPartial<vtkm::Float32>>::composite<vtkm::Float32>(std::vector<PartialImage<vtkm::Float32>> &);

template 
PartialImage<vtkm::Float64> 
Compositor<AbsorptionPartial<vtkm::Float32>>::composite<vtkm::Float64>(std::vector<PartialImage<vtkm::Float64>> &);
*/
} // namespace rover
