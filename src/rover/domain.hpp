#ifndef rover_domain_h
#define rover_domain_h

#include <memory>

#include <engine.hpp>
#include <rover_types.hpp>
#include <vtkm_typedefs.hpp>

namespace rover {

class Domain
{
public:
  Domain();
  ~Domain();
  vtkmDataSet get_data_set();
  void trace(Ray32 &rays);
  void trace(Ray64 &rays);
  void init_rays(Ray32 &rays);
  void init_rays(Ray64 &rays);
  void set_data_set(vtkmDataSet &dataset);
  void set_render_settings(const RenderSettings &setttings);
  void set_primary_range(const vtkmRange &range);
  void set_composite_background(bool on);
  vtkmRange get_primary_range();
protected:
  std::shared_ptr<Engine> m_engine;
  vtkmDataSet             m_data_set;
  RenderMode              m_render_mode;

}; // class domain
} // namespace rover
#endif
