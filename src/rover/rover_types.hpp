#ifndef rover_types_h
#define rover_types_h

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

struct RenderSettings 
{
  RenderMode     m_render_mode;
  ScatteringType m_scattering_type;
  RayScope       m_ray_scope;
  //
  // Default settings
  // 
  RenderSettings()
  {
    m_render_mode     = volume;
    m_scattering_type = non_scattering;
    m_ray_scope       = global_rays;
  }

};
} // namespace rover
#endif
