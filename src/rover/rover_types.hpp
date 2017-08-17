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
    : m_divide_abs_by_emmision(false)
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
  
  void print()
  {
    std::cout<<"******* Settings *********\n";
    if(m_render_mode == energy)
    {
      std::cout<<"Render Mode: ENERGY\n";
    }
    else
    {
      std::cout<<"Render Mode: VOLUME\n";
    }

    std::cout<<"Primary field: "<<m_primary_field<<"\n";
    std::cout<<"Secondary field: "<<m_secondary_field<<"\n";
     
    std::cout<<"*************************\n";


  }
};

template<typename FloatType>
struct PartialImage
{
  int                                      m_height;
  int                                      m_width;
  IdHandle                                 m_pixel_ids;
  vtkmRayTracing::ChannelBuffer<FloatType> m_buffer;         //holds either color or absorption
  vtkmRayTracing::ChannelBuffer<FloatType> m_emission_buffer;
  vtkm::cont::ArrayHandle<FloatType>       m_distances;

  void print_pixel(const int x, const int y)
  {
    const int size = m_pixel_ids.GetPortalControl().GetNumberOfValues();
    const int num_channels = m_buffer.GetNumChannels();
    int debug = m_width * ( m_height - y) + x; 

    for(int i = 0; i < size; ++i)
    {
      if(m_pixel_ids.GetPortalControl().Get(i) == debug)
      {
        int offset = i * num_channels;
        for(int j = 0; j < num_channels ; ++j)
        {
          std::cout<<m_buffer.Buffer.GetPortalControl().Get(offset + j)<<" ";
        }
        std::cout<<"\n";
      }
    }

  
  }// print

  void make_red_pixel(const int x, const int y)
    {
      const int size = m_pixel_ids.GetPortalControl().GetNumberOfValues();
      const int num_channels = m_buffer.GetNumChannels();
      int debug = m_width * ( m_height - y) + x; 

      for(int i = 0; i < size; ++i)
      {
        if(m_pixel_ids.GetPortalControl().Get(i) == debug)
        {
          int offset = i * num_channels;
          m_buffer.Buffer.GetPortalControl().Set(offset , 1.f);
          for(int j = 1; j < num_channels -1; ++j)
          {
            m_buffer.Buffer.GetPortalControl().Set(offset + j, 0.f);
          }
          m_buffer.Buffer.GetPortalControl().Set(offset + num_channels-1,1.f);
        }
      }
    }
};

} // namespace rover
#endif
