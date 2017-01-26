#include <ray_generators/camera_generator.hpp>
#include <scheduler.hpp>
#include <volume_engine.hpp>
#include <energy_engine.hpp>
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
  m_engine = new VolumeEngine(); 
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

  m_render_settings = render_settings;
  // 
  // Create the correct engine
  //
  if(m_engine) delete m_engine;
  m_engine = NULL;

  if(m_render_settings.m_render_mode == volume)
  {
    m_engine = new VolumeEngine(); 

  }
  else if(m_render_settings.m_render_mode == energy)
  {
    m_engine =  new EnergyEngine();
  }
  else if(m_render_settings.m_render_mode == surface)
  {
    std::cout<<"ray tracing not implemented\n";
  }

  if(m_engine == NULL)
  {
    ROVER_ERROR("Unable to create the appropriate engine");
    throw RoverException("Fatal Error: schedular unable to create the apporpriate engine\n");
  }
  ROVER_INFO("Primary field "<<render_settings.m_primary_field);
  m_engine->set_primary_field(render_settings.m_primary_field);
  m_engine->set_secondary_field(render_settings.m_secondary_field);
  m_engine->set_color_table(render_settings.m_color_table);

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
  
  return get_vtkm_ptr(m_result_handle);
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

  if(m_render_settings.m_render_mode == energy)
  {
    std::cout<<"engergy not implemented\n";
  }
  else
  {
    // TODO: add some conversion to uchar probably withing the "channel buffer"
    assert(rays.Buffers.at(0).GetNumChannels() == 4); 
    int buffer_size = m_ray_generator->get_size();
    m_result_handle = rays.Buffers.at(0).ExpandBuffer(rays.PixelIdx, buffer_size).Buffer;
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
void Scheduler<FloatType>::save_result(std::string file_name) const
{
  int height = 0;
  int width = 0;
  m_ray_generator->get_dims(height, width);
  assert( height > 0 );
  assert( width > 0 );
  ROVER_INFO("Saving file " << height << " "<<width);
  PNGEncoder encoder;
  FloatType * buffer = get_vtkm_ptr(m_result_handle);

  encoder.Encode(buffer, width, height);
  encoder.Save(file_name);
  
}

template<typename FloatType>
void Scheduler<FloatType>::set_ray_generator(RayGenerator<FloatType> *ray_generator)
{
  m_ray_generator = ray_generator;
}
// Explicit instantiation
template class Scheduler<vtkm::Float32>; template class Scheduler<vtkm::Float64>;
}; // namespace rover
