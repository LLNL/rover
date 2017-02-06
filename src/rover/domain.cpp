#include <domain.hpp>
#include <volume_engine.hpp>
#include <energy_engine.hpp>

namespace rover {
Domain::Domain()
{
  m_engine = new VolumeEngine(); 
}

Domain::~Domain()
{
  delete m_engine;
}

Domain::set_render_settings(const RenderSettings &settings)
{
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
    throw RoverException("Fatal Error: domain unable to create the apporpriate engine\n");
  }
  ROVER_INFO("Primary field "<<render_settings.m_primary_field);
  m_engine->set_primary_field(render_settings.m_primary_field);
  m_engine->set_secondary_field(render_settings.m_secondary_field);
  m_engine->set_color_table(render_settings.m_color_table);

}

void 
Domain::set_data_set(vtkmDataSet &dataset)
{
  m_engine.set_data_set(dataset);
}

vtkmDataSet
Domain::get_data_set()
{
  return m_data_set;
}

} // namespace rover 
