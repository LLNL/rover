#include <schedular.hpp>
#include <volume_engine.hpp>
namespace rover {

Schedular::Schedular()
{
  m_engine = NULL;
}

Schedular::~Schedular()
{
  if(m_engine) delete m_engine;
}

void
Schedular::set_render_settings(const RenderSettings render_render_settings)
{
  //
  //  In the serial schedular, the only setting that matter are 
  //  m_render_mode and m_scattering_mode
  //

  // 
  // Create the correct engine
  //
  if(m_engine) delete m_engine;

  if(m_render_settings.m_render_mode == volume)
  {
    m_engine = new VolumeEngine(); 
  }
  else if(m_render_settings.m_render_mode == energy)
  {
    std::cout<<"engergy not implemented\n";
  }
  else if(m_render_settings.m_render_mode == surface)
  {
    std::cout<<"ray tracing not implemented\n";
  }
}
void 
Schedular::set_data_set(vtkmDataSet &dataset)
{
  m_engine->set_data_set(dataset);
}

vtkmDataSet
Schedular::get_data_set() const
{
  return m_data_set;
}

RenderSettings
Schedular::get_render_settings() const
{
  return m_render_settings;
}

//
// in the other schedulars this method will be far from trivial
//
void 
Schedular::trace_rays(Ray32 &rays)
{
  m_engine->trace(rays);
}

void 
Schedular::trace_rays(Ray64 &rays)
{
  m_engine->trace(rays);
}



}; // namespace rover
