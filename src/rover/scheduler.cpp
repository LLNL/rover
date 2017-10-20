
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
  m_ray_generator = NULL;
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
}

template<typename FloatType>
int
Scheduler<FloatType>::get_global_channels()
{

  int num_channels = 0;
  if(m_partial_images.size() > 0)
  {
    num_channels = m_partial_images[0].m_buffer.GetNumChannels();
  }
#ifdef PARALLEL
  vtkmTimer timer;
  double time = 0;
  int mpi_num_channels;
  MPI_Allreduce(&num_channels, &mpi_num_channels, 1, MPI_INT, MPI_MAX, m_comm_handle);
  num_channels = mpi_num_channels;
  time = timer.GetElapsedTime();
  DataLogger::GetInstance()->AddLogData("get_global_channels_all_reduce", time);
#endif

  ROVER_INFO("Global number of channels"<<num_channels);
  assert(num_channels > 0);
  return num_channels;
}

template<typename FloatType>
void
Scheduler<FloatType>::set_global_scalar_range()
{

  vtkmTimer timer;
  double time = 0;

  const int num_domains = static_cast<int>(m_domains.size());

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

  time = timer.GetElapsedTime();
  DataLogger::GetInstance()->AddLogData("set_global_scalar_range", time);
}

template<typename FloatType>
void
Scheduler<FloatType>::set_global_bounds()
{
  vtkmTimer timer;
  double time = 0;

  const int num_domains = static_cast<int>(m_domains.size());

  vtkm::Bounds global_bounds;

  for(int i = 0; i < num_domains; ++i) 
  {
    vtkm::Bounds local_bounds = m_domains[i].get_domain_bounds();
    global_bounds.Include(local_bounds);
  }

#ifdef PARALLEL

  double x_min = global_bounds.X.Min;
  double x_max = global_bounds.X.Max;
  double y_min = global_bounds.Y.Min;
  double y_max = global_bounds.Y.Max;
  double z_min = global_bounds.Z.Min;
  double z_max = global_bounds.Z.Max;
  double global_x_min = 0;
  double global_x_max = 0;
  double global_y_min = 0;
  double global_y_max = 0;
  double global_z_min = 0;
  double global_z_max = 0;

  MPI_Allreduce((void *)(&x_min),
                (void *)(&global_x_min), 
                1,
                MPI_DOUBLE,
                MPI_MIN,
                m_comm_handle);

  MPI_Allreduce((void *)(&x_max),
                (void *)(&global_x_max),
                1,
                MPI_DOUBLE,
                MPI_MAX,
                m_comm_handle);

  MPI_Allreduce((void *)(&y_min),
                (void *)(&global_y_min), 
                1,
                MPI_DOUBLE,
                MPI_MIN,
                m_comm_handle);

  MPI_Allreduce((void *)(&y_max),
                (void *)(&global_y_max),
                1,
                MPI_DOUBLE,
                MPI_MAX,
                m_comm_handle);

  MPI_Allreduce((void *)(&z_min),
                (void *)(&global_z_min), 
                1,
                MPI_DOUBLE,
                MPI_MIN,
                m_comm_handle);

  MPI_Allreduce((void *)(&z_max),
                (void *)(&global_z_max),
                1,
                MPI_DOUBLE,
                MPI_MAX,
                m_comm_handle);

  global_bounds.X.Min = global_x_min;
  global_bounds.X.Max = global_x_max;
  global_bounds.Y.Min = global_y_min;
  global_bounds.Y.Max = global_y_max;
  global_bounds.Z.Min = global_z_min;
  global_bounds.Z.Max = global_z_max;
#endif

  ROVER_INFO("Global bounds "<<global_bounds);

  for(int i = 0; i < num_domains; ++i) 
  {
    m_domains[i].set_global_bounds(global_bounds);
  }
  time = timer.GetElapsedTime();
  DataLogger::GetInstance()->AddLogData("set_global_bounds", time);
}

template<typename FloatType>
void 
Scheduler<FloatType>::add_data_set(vtkmDataSet &dataset)
{
  ROVER_INFO("Adding domain "<<m_domains.size());
  Domain domain;
  domain.set_data_set(dataset);
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
  return get_vtkm_ptr(m_partial_images.at(-1).m_buffer.Buffer);
}

//
// in the other schedulars this method will be far from trivial
//
template<typename FloatType>
void 
Scheduler<FloatType>::trace_rays()
{
  ROVER_INFO("tracing_rays");
  vtkmTimer tot_timer;
  vtkmTimer timer;
  double time = 0;
  DataLogger::GetInstance()->OpenLogEntry("schedule_trace");

  if(m_ray_generator == NULL)
  {
    throw RoverException("Error: ray generator must be set before execute is called");
  }

  m_ray_generator->reset();
  // TODO while (m_geerator.has_rays())
  ROVER_INFO("Tracing rays");

  int height = 0 ;
  int width = 0;

  m_ray_generator->get_dims(height, width);
  
  //
  // ensure that the render settings are set
  //
  // TODO: make copy constructor so the mesh stuctures are not rebuilt when moving from
  //       volume to energy and vice versa
  const int num_domains = static_cast<int>(m_domains.size());
  ROVER_INFO("scheduer set render settings for "<<num_domains<<" domains ");
  for(int i = 0; i < num_domains; ++i) 
  {
    m_domains[i].set_render_settings(m_render_settings);
  }
  
  time = timer.GetElapsedTime();
  DataLogger::GetInstance()->AddLogData("beginning", time);

  this->set_global_scalar_range();
  this->set_global_bounds();

  //
  // check to see if we have to composite
  bool do_compositing = num_domains > 1;
#ifdef PARALLEL
  do_compositing = true;
#endif

  vtkmTimer trace_timer;
  for(int i = 0; i < num_domains; ++i)
  {
    vtkmTimer domain_timer;
    std::stringstream domain_s;
    domain_s<<"trace_domain_"<<i;
    DataLogger::GetInstance()->OpenLogEntry(domain_s.str());
    vtkmLogger::GetInstance()->Clear();
    if(dynamic_cast<CameraGenerator<FloatType>*>(m_ray_generator) != NULL)
    {
      //
      // Setting the coordinate system miminizes the number of rays generated
      //
      CameraGenerator<FloatType> *generator = dynamic_cast<CameraGenerator<FloatType>*>(m_ray_generator);
      generator->set_coordinates(m_domains[i].get_data_set().GetCoordinateSystem());
    }
    ROVER_INFO("Generating rays for domian "<<i);

    timer.Reset();
  
    vtkmRayTracing::Ray<FloatType> rays = m_ray_generator->get_rays();
    ROVER_INFO("Generated "<<rays.NumRays<<" rays");
    m_domains[i].init_rays(rays);
    //
    // add path lengths if they were requested
    //
    if(m_render_settings.m_path_lengths)
    {
      rays.AddBuffer(1, "path_lengths");
      rays.GetBuffer("path_lengths").InitConst(0);
    }
    time = timer.GetElapsedTime();
    DataLogger::GetInstance()->AddLogData("domain_init_rays", time);

    if(do_compositing)
    {
      m_domains[i].set_composite_background(false);
    }

    ROVER_INFO("Tracing domain "<<i);

    timer.Reset();
    m_domains[i].trace(rays);
    time = timer.GetElapsedTime();
    DataLogger::GetInstance()->AddLogData("domain_trace", time);
    DataLogger::GetInstance()->GetStream()<<vtkmLogger::GetInstance()->GetStream().str();

    //
    // Create a partial image result from the completed rays
    //

    ROVER_INFO("Schedule: creating partial image in domain "<<i);
    PartialImage<FloatType> partial_image;
    partial_image.m_pixel_ids = rays.PixelIdx;
    partial_image.m_distances = rays.MinDistance;
    partial_image.m_width = width;
    partial_image.m_height = height;

    if(m_render_settings.m_path_lengths)
    {
      partial_image.m_path_lengths = rays.GetBuffer("path_lengths").Buffer;
    }

    if(m_render_settings.m_render_mode == energy)
    {
      partial_image.m_buffer = rays.Buffers.at(0);
      ROVER_INFO("  buffer size "     <<partial_image.m_buffer.GetSize());
      ROVER_INFO("  buffer channels " <<partial_image.m_buffer.GetNumChannels());
      ROVER_INFO("  buffer length    "<<partial_image.m_buffer.GetBufferLength());
      assert(partial_image.m_buffer.GetNumChannels() > 0);
      if(m_render_settings.m_secondary_field != "")
      {
        partial_image.m_intensities = rays.GetBuffer("emission");
        ROVER_INFO(" i buffer size "     <<partial_image.m_intensities.GetSize());
        ROVER_INFO(" i buffer channels " <<partial_image.m_intensities.GetNumChannels());
        ROVER_INFO(" i  buffer length   "<<partial_image.m_intensities.GetBufferLength());
        assert(partial_image.m_intensities.GetNumChannels() > 0);
      }
    }
    else
    {
      // TODO: add some conversion to uchar probably withing the "channel buffer"
      assert(rays.Buffers.at(0).GetNumChannels() == 4); 
      partial_image.m_buffer = rays.Buffers.at(0);
    }
    
    timer.Reset();
    m_partial_images.push_back(partial_image);

    time = timer.GetElapsedTime(); 
    DataLogger::GetInstance()->AddLogData("domain_push_back", time);

    time = domain_timer.GetElapsedTime();
    DataLogger::GetInstance()->CloseLogEntry(time);
    ROVER_INFO("Schedule: done tracing domain "<<i);
  }// for each domain
  timer.Reset();
  time = trace_timer.GetElapsedTime(); 
  DataLogger::GetInstance()->AddLogData("total_trace", time);
  //
  // Add dummy partial image if we had no domains
  //
  //int num_channels = this->get_global_channels(); 
  int num_channels = m_partial_images[0].m_buffer.GetNumChannels();
    
  vtkmTimer t1;
  if(num_domains == 0)
  {
    PartialImage<FloatType> partial_image;
    partial_image.m_width = width;
    partial_image.m_height = height;
    partial_image.m_buffer = 
      vtkm::rendering::raytracing::ChannelBuffer<FloatType>(num_channels, 0);
    if(m_render_settings.m_secondary_field != "")
    {
      partial_image.m_intensities = 
        vtkm::rendering::raytracing::ChannelBuffer<FloatType>(num_channels, 0);
    }
    m_partial_images.push_back(partial_image);
  }
  DataLogger::GetInstance()->AddLogData("blank_image", t1.GetElapsedTime());
  t1.Reset();

  if(m_background.size() == 0)
  {
    this->create_default_background(num_channels);
  }

  DataLogger::GetInstance()->AddLogData("default_bg", t1.GetElapsedTime());
  t1.Reset();

  time = timer.GetElapsedTime();
  DataLogger::GetInstance()->AddLogData("mid", time);
  timer.Reset();

  timer.Reset();
  if(do_compositing)
  {
    //TODO: composite
    if(m_render_settings.m_render_mode == volume)
    {
      Compositor<VolumePartial<FloatType>> compositor;
      compositor.set_background(m_background);
#ifdef PARALLEL
      compositor.set_comm_handle(m_comm_handle);
#endif
      m_result = compositor.composite(m_partial_images);
    }
    else
    {
      if(m_render_settings.m_secondary_field != "")
      {
        Compositor<EmissionPartial<FloatType>> compositor;
        compositor.set_background(m_background);
#ifdef PARALLEL
        compositor.set_comm_handle(m_comm_handle);
#endif
        m_result = compositor.composite(m_partial_images);
      }
      else
      {
        Compositor<AbsorptionPartial<FloatType>> compositor;
        compositor.set_background(m_background);
#ifdef PARALLEL
        compositor.set_comm_handle(m_comm_handle);
#endif
        m_result = compositor.composite(m_partial_images);
      }
    }
    ROVER_ERROR("Schedule: compositing complete");
  }
  else if(num_domains == 1)
  {
    //
    // if enegry and no compositing we need to add
    // the energy buffer to the absorption buffer
    //
    m_partial_images[0].m_source_sig = m_background;
    if(m_render_settings.m_render_mode == energy &&
       m_render_settings.m_secondary_field != "")
    {
      m_partial_images[0].add_source_sig();
    }
    ROVER_INFO("Single domain output "<<m_partial_images[0].m_width
               <<" x "<<m_partial_images[0].m_height);
    if(m_render_settings.m_secondary_field == "")
    {
      // TODO: make a copy
      m_partial_images[0].m_intensities = m_partial_images[0].m_buffer;
    }
    m_result = m_partial_images[0];
  }
  else 
  {
    ROVER_ERROR("Invalid number of domains: "<<num_domains);
  }

  time = timer.GetElapsedTime();
  DataLogger::GetInstance()->AddLogData("compositing", time);
  timer.Reset();

  m_partial_images.clear();
  time = timer.GetElapsedTime();
  DataLogger::GetInstance()->AddLogData("clear", time);

  double tot_time = tot_timer.GetElapsedTime();
  DataLogger::GetInstance()->CloseLogEntry(tot_time);
  ROVER_INFO("Schedule: end of trace");
}

template<typename FloatType>
Image<FloatType> 
Scheduler<FloatType>::get_result()
{
  return m_result;
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
    const int num_channels = m_result.get_num_channels();
    ROVER_INFO("Saving "<<num_channels<<" channels ");
    for(int i = 0; i < num_channels; ++i)
    {
      std::stringstream sstream;
      sstream<<file_name<<"_"<<i<<".png";
      m_result.normalize_intensity(i);
      FloatType * buffer 
        = get_vtkm_ptr(m_result.get_intensity(i));

      encoder.EncodeChannel(buffer, width, height);
      encoder.Save(sstream.str());
    }
  }
  else
  {
     
    assert(m_result.get_num_channels() == 4);
    vtkm::cont::ArrayHandle<FloatType> colors;
    colors = m_result.flatten_intensities();
    FloatType * buffer 
      = get_vtkm_ptr(colors);
    
    encoder.Encode(buffer, width, height);
    encoder.Save(file_name + ".png");
  }

  if(m_render_settings.m_path_lengths)
  {
     std::stringstream sstream;
     sstream<<file_name<<"_paths"<<".png";
     m_result.normalize_paths();
     FloatType * buffer 
       = get_vtkm_ptr(m_result.get_path_lengths());

     encoder.EncodeChannel(buffer, width, height);
     encoder.Save(sstream.str());
  }
  
}

template<typename FloatType>
void Scheduler<FloatType>::set_ray_generator(RayGenerator<FloatType> *ray_generator)
{
  m_ray_generator = ray_generator;
}

template<typename FloatType>
void Scheduler<FloatType>::set_background(const std::vector<FloatType> &background)
{
  m_background = background;
}

template<typename FloatType>
void Scheduler<FloatType>::clear_data_sets()
{
  m_domains.clear();
}

template<typename FloatType>
void Scheduler<FloatType>::create_default_background(const int num_channels)
{
  m_background.resize(num_channels);
  for(int i = 0; i < num_channels; ++i)
  {
    m_background[i] = 1.f;
  }
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
template class Scheduler<vtkm::Float32>; 
template class Scheduler<vtkm::Float64>;
}; // namespace rover
