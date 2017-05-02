#include <domain.hpp>
#include <volume_engine.hpp>
#include <energy_engine.hpp>
#include <rover_exceptions.hpp>
#include <utils/rover_logging.hpp>

namespace rover {
Domain::Domain()
{
  m_engine = std::make_shared<VolumeEngine>(); 
}

Domain::~Domain()
{
}

void
Domain::set_render_settings(const RenderSettings &settings)
{
  // 
  // Create the correct engine
  //

  if(m_render_mode != volume && settings.m_render_mode == volume)
  {
    m_engine = std::make_shared<VolumeEngine>(); 

  }
  else if(m_render_mode != energy && settings.m_render_mode == energy)
  {
    m_engine = std::make_shared<EnergyEngine>();
  }
  else if(m_render_mode != surface && settings.m_render_mode == surface)
  {
    std::cout<<"ray tracing not implemented\n";
  }
  else
  {
    //ROVER_ERROR("Unable to create the appropriate engine");
    //throw RoverException("Fatal Error: domain unable to create the apporpriate engine\n");
  }

  m_render_mode = settings.m_render_mode; 

  ROVER_INFO("Primary field " << settings.m_primary_field);

  m_engine->set_primary_field(settings.m_primary_field);
  m_engine->set_secondary_field(settings.m_secondary_field);
  m_engine->set_color_table(settings.m_color_table);


}

void 
Domain::set_data_set(vtkmDataSet &dataset)
{
  m_engine->set_data_set(dataset);
  m_data_set = dataset;
}

vtkmDataSet
Domain::get_data_set()
{
  return m_data_set;
}

void
Domain::init_rays(Ray32 &rays)
{
  m_engine->init_rays(rays);
}

void
Domain::init_rays(Ray64 &rays)
{
  m_engine->init_rays(rays);
}

void
Domain::trace(Ray32 &rays)
{
  m_engine->trace(rays);
}

void
Domain::trace(Ray64 &rays)
{
  m_engine->trace(rays);
}

void 
Domain::set_primary_range(const vtkmRange &range)
{
  m_engine->set_primary_range(range);
}

void 
Domain::set_composite_background(bool on)
{
  m_engine->set_composite_background(on);
}

vtkmRange
Domain::get_primary_range()
{
  return m_engine->get_primary_range();
}

} // namespace rover 
