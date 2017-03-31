
#include <assert.h>
#include <compositing/compositor.hpp>
#include <scheduler.hpp>
#include <utils/png_encoder.hpp>
#include <utils/rover_logging.hpp>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm_typedefs.hpp>
#include <ray_generators/camera_generator.hpp>
#include <rover_exceptions.hpp>


namespace rover {

template<typename FloatType>
Scheduler<FloatType>::Scheduler()
{
}

template<typename FloatType>
Scheduler<FloatType>::~Scheduler()
{
}

template<typename FloatType>
void
Scheduler<FloatType>::set_render_settings(const RenderSettings render_settings)
{
  //
  //  In the serial schedular, the only setting that matter are 
  //  m_render_mode and m_scattering_mode
  //
  m_render_settings = render_settings;
  // TODO: make copy constructor so the mesh stuctures are not rebuilt when moving from
  //       volume to energy and vice versa
  const int num_domains = static_cast<int>(m_domains.size());
  for(int i = 0; i < num_domains; ++i) 
  {
    m_domains[i].set_render_settings(m_render_settings);
  }
}

template<typename FloatType>
void
Scheduler<FloatType>::set_global_scalar_range()
{

  const int num_domains = static_cast<int>(m_domains.size());
  if(num_domains == 1)
  {
    // Nothing to do
    return;
  }
  vtkmRange global_range;

  for(int i = 0; i < num_domains; ++i) 
  {
    vtkmRange local_range = m_domains[i].get_primary_range();
    global_range.Include(local_range);
  }
#ifdef PARALLEL
  double rank_min = global_range.Min;
  double rank_max = global_range.Max;
  double mpi_min;
  double mpi_max;
  MPI_Allreduce(&rank_min, &mpi_min, 1, MPI_DOUBLE, MPI_MIN, m_comm_handle);
  MPI_Allreduce(&rank_max, &mpi_max, 1, MPI_DOUBLE, MPI_MAX, m_comm_handle);
  global_range.Min = mpi_min;
  global_range.Max = mpi_max;
#endif

  ROVER_INFO("Global scalar range "<<global_range);

  for(int i = 0; i < num_domains; ++i) 
  {
    m_domains[i].set_primary_range(global_range);
  }
}

template<typename FloatType>
void 
Scheduler<FloatType>::add_data_set(vtkmDataSet &dataset)
{
  ROVER_INFO("Adding domain "<<m_domains.size());
  Domain domain;
  domain.set_render_settings(m_render_settings);
  domain.set_data_set(dataset);
  dataset.PrintSummary(std::cout); 
  domain.get_data_set().PrintSummary(std::cout); 
  m_domains.push_back(domain);
}

template<typename FloatType>
vtkmDataSet
Scheduler<FloatType>::get_data_set(const int &domain)
{
  return m_domains.at(domain).get_data_set();
}

template<typename FloatType>
RenderSettings
Scheduler<FloatType>::get_render_settings() const
{
  return m_render_settings;
}

template<typename FloatType>
FloatType *
Scheduler<FloatType>::get_color_buffer()
{
  if(m_render_settings.m_render_mode == energy)
  {
    throw RoverException("cannot call get_color_buffer while using energy mode");
  }
  ROVER_ERROR("This should not be called"); 
  return get_vtkm_ptr(m_partial_images.at(100).m_buffer.Buffer);
}

//
// in the other schedulars this method will be far from trivial
//
template<typename FloatType>
void 
Scheduler<FloatType>::trace_rays()
{

  // TODO while (m_geerator.has_rays())
  ROVER_INFO("Tracing rays");

  int height = 0;
  int width = 0;
  m_ray_generator->get_dims(height, width);
  
  this->set_global_scalar_range();

  const int num_domains = static_cast<int>(m_domains.size());
  //
  // check to see if we have to composite
  bool do_compositing = num_domains > 1;
#ifdef PARALLEL
  do_compositing = true;
#endif

  for(int i = 0; i < num_domains; ++i)
  {
    if(dynamic_cast<CameraGenerator<FloatType>*>(m_ray_generator) != NULL)
    {
      //
      // Setting the coordinate system miminizes the number of rays generated
      //
      CameraGenerator<FloatType> *generator = dynamic_cast<CameraGenerator<FloatType>*>(m_ray_generator);
      m_domains[i].get_data_set().PrintSummary(std::cout);
      generator->set_coordinates(m_domains[i].get_data_set().GetCoordinateSystem());
    }
    ROVER_INFO("Generating rays for domian "<<i);
    vtkmRayTracing::Ray<FloatType> rays = m_ray_generator->get_rays();
    if(do_compositing)
    {
      m_domains[i].set_composite_background(false);
    }
    ROVER_INFO("Tracing domain "<<i);
    m_domains[i].trace(rays);

    PartialImage<FloatType> partial_image;
    partial_image.m_pixel_ids = rays.PixelIdx;
    partial_image.m_distances = rays.MinDistance;
    partial_image.m_width = width;
    partial_image.m_height = height;

    if(m_render_settings.m_render_mode == energy)
    {
      partial_image.m_buffer = rays.Buffers.at(0);
      assert(partial_image.m_buffer.GetNumChannels() > 0);
    }
    else
    {
      // TODO: add some conversion to uchar probably withing the "channel buffer"
      assert(rays.Buffers.at(0).GetNumChannels() == 4); 

      partial_image.m_buffer = rays.Buffers.at(0);

    }
    m_partial_images.push_back(partial_image);
  }// for each domain

  if(do_compositing)
  {
    //TODO: composite
    if(m_render_settings.m_render_mode == volume)
    {
      Compositor<VolumePartial> compositor;
#ifdef PARALLEL
      compositor.set_comm_handle(m_comm_handle);
#endif
      m_result = compositor.composite(m_partial_images);
    }
    else
    {
      Compositor<AbsorptionPartial<FloatType>> compositor;
#ifdef PARALLEL
     compositor.set_comm_handle(m_comm_handle);
#endif
      m_result = compositor.composite(m_partial_images);
    }
  }
  else if(num_domains == 1)
  {
    m_result = m_partial_images[0];
  }
  else 
  {
    ROVER_ERROR("Invalid number of domains: "<<num_domains);
  }
  m_partial_images.clear();
}

template<typename FloatType>
void Scheduler<FloatType>::save_result(std::string file_name) 
{
  int height = 0;
  int width = 0;
  int buffer_size = m_ray_generator->get_size();
  m_ray_generator->get_dims(height, width);
  assert( height > 0 );
  assert( width > 0 );
  ROVER_INFO("Saving file " << height << " "<<width);
  PNGEncoder encoder;

  if(m_render_settings.m_render_mode == energy)
  {
    const int num_channels = m_result.m_buffer.GetNumChannels();
    ROVER_INFO("Saving "<<num_channels<<" channels ");
    for(int i = 0; i < num_channels; ++i)
    {
      std::stringstream sstream;
      sstream<<file_name<<"_"<<i<<".png";
      vtkmRayTracing::ChannelBuffer<FloatType> channel = m_result.m_buffer.GetChannel( i );
      const bool invert = true;
      channel.Normalize(invert);
      const FloatType default_value = 0.f;
      vtkmRayTracing::ChannelBuffer<FloatType>  expand 
        = channel.ExpandBuffer(m_result.m_pixel_ids, buffer_size, default_value);

      FloatType * buffer 
        = get_vtkm_ptr(expand.Buffer);

      encoder.EncodeChannel(buffer, width, height);
      encoder.Save(sstream.str());
    }
  }
  else
  {
      
    vtkmRayTracing::ChannelBuffer<FloatType>  expand 
      = m_result.m_buffer.ExpandBuffer(m_result.m_pixel_ids, buffer_size);
    FloatType * buffer 
      = get_vtkm_ptr(expand.Buffer);
    
    assert(m_result.m_buffer.GetNumChannels() == 4);
    encoder.Encode(buffer, width, height);
    encoder.Save(file_name + ".png");
  }
  
}

template<typename FloatType>
void Scheduler<FloatType>::set_ray_generator(RayGenerator<FloatType> *ray_generator)
{
  m_ray_generator = ray_generator;
}

template<typename FloatType>
void Scheduler<FloatType>::clear_data_sets()
{
  m_domains.clear();
}

#ifdef PARALLEL
template<typename FloatType>
void Scheduler<FloatType>::set_comm_handle(MPI_Comm comm_handle)
{
  m_comm_handle = comm_handle;
}
#endif
//
// Explicit instantiation
template class Scheduler<vtkm::Float32>; template class Scheduler<vtkm::Float64>;
}; // namespace rover
