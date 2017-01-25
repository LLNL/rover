#ifndef rover_types_h
#define rover_types_h

#include <vtkm_typedefs.hpp>

namespace rover {
// this could be ray tracing(surface) / volume rendering / energy 
enum RenderMode 
{ 
  volume, // standard volume rendering (3d)
  energy, // radiography with enegry bins (3d)
  surface // 
};

enum ScatteringType 
{
  scattering,    // ray can be scattered in random directions
  non_scattering // rays cannot be scattered
};
//
// Ray scope is only meaningful in parallel and is ignored otherwise
//
enum RayScope 
{ 
  global_rays,  // rays can exist in one ore more domains at one time
  local_rays    // ran only exist in a single domain st any given time
};
//
// Volume rendering specific settigns
//
struct VolumeSettings
{
  int m_num_samples; // approximate number of samples per ray
  VolumeSettings()
    : m_num_samples(200)
  {}
};
//
// Energy specific settings
//
struct EnergySettings
{
  bool m_divide_abs_by_emmision;
  EnergySettings()
    : m_devide_abs_by_emmision(false)
  {}
};

struct RenderSettings 
{
  RenderMode     m_render_mode;
  ScatteringType m_scattering_type;
  RayScope       m_ray_scope;
  vtkmColorTable m_color_table; 
  std::string    m_primary_field;
  std::string    m_secondary_field;
  VolumeSettings m_volume_settings;
  EnergySettings m_energy_settings;
  //
  // Default settings
  // 
  RenderSettings()
    : m_color_table("cool2warm")
  {
    m_render_mode     = volume;
    m_scattering_type = non_scattering;
    m_ray_scope       = global_rays;
  }

};
} // namespace rover
#endif
