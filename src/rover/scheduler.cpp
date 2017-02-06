#include <ray_generators/camera_generator.hpp>
#include <scheduler.hpp>
#include <vtkm_typedefs.hpp>
#include <utils/png_encoder.hpp>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <rover_exceptions.hpp>
#include <assert.h>
#include <utils/rover_logging.hpp>
namespace rover {

template<typename FloatType>
Scheduler<FloatType>::Scheduler()
{
}

template<typename FloatType>
Scheduler<FloatType>::~Scheduler()
{
  if(m_engine) delete m_engine;
}

template<typename FloatType>
void
Scheduler<FloatType>::set_render_settings(const RenderSettings render_settings)
{
  //
  //  In the serial schedular, the only setting that matter are 
  //  m_render_mode and m_scattering_mode
  //

  
}

template<typename FloatType>
void 
Scheduler<FloatType>::set_data_set(vtkmDataSet &dataset)
{
  m_engine->set_data_set(dataset);
}

template<typename FloatType>
vtkmDataSet
Scheduler<FloatType>::get_data_set() const
{
  return m_data_set;
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
  
  return get_vtkm_ptr(m_result_buffer.Buffer);
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
  vtkmRayTracing::Ray<FloatType> rays = m_ray_generator->get_rays();
  m_engine->trace(rays);

  m_pixel_ids = rays.PixelIdx;

  if(m_render_settings.m_render_mode == energy)
  {
    m_result_buffer = rays.Buffers.at(0);
    assert(m_result_buffer.GetNumChannels() > 0);
  }
  else
  {
    // TODO: add some conversion to uchar probably withing the "channel buffer"
    assert(rays.Buffers.at(0).GetNumChannels() == 4); 
    m_result_buffer = rays.Buffers.at(0);

    /*
    int height = 0;
    int width = 0;
    m_ray_generator->get_dims(height, width);
    vtkm::rendering::CanvasRayTracer canvas(width, height);
    vtkm::rendering::Camera cam;
    canvas.WriteToCanvas(rays, cam);
    canvas.SaveAs("test.pnm"); 
    */
  }
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
    const int num_channels = m_result_buffer.GetNumChannels();
    ROVER_INFO("Saving "<<num_channels<<" channels ");
    for(int i = 0; i < num_channels; ++i)
    {
      std::stringstream sstream;
      sstream<<file_name<<"_"<<i<<".png";
      vtkmRayTracing::ChannelBuffer<FloatType> channel = m_result_buffer.GetChannel( i );
      const bool invert = true;
      channel.Normalize(invert);

      vtkmRayTracing::ChannelBuffer<FloatType>  expand = channel.ExpandBuffer(m_pixel_ids, buffer_size);
      FloatType * buffer 
        = get_vtkm_ptr(expand.Buffer);
      encoder.EncodeChannel(buffer, width, height);
      encoder.Save(sstream.str());
    }
  }
  else
  {
      
    vtkmRayTracing::ChannelBuffer<FloatType>  expand = m_result_buffer.ExpandBuffer(m_pixel_ids, buffer_size);
    FloatType * buffer 
      = get_vtkm_ptr(expand.Buffer);
    
    assert(m_result_buffer.GetNumChannels() == 4);
    encoder.Encode(buffer, width, height);
    encoder.Save(file_name + ".png");
  }
  
}

template<typename FloatType>
void Scheduler<FloatType>::set_ray_generator(RayGenerator<FloatType> *ray_generator)
{
  m_ray_generator = ray_generator;
}
// Explicit instantiation
template class Scheduler<vtkm::Float32>; template class Scheduler<vtkm::Float64>;
}; // namespace rover
