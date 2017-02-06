#ifndef rover_scheduler_h
#define rover_scheduler_h

#include <domain.hpp>
#include <engine.hpp>
#include <rover_types.hpp>
#include <ray_generators/ray_generator.hpp>
#include <vtkm_typedefs.hpp>
//
// Scheduler types:
//  static: all ranks gets all rays
//  normal compositing -
//    back to front (energy): absorbtion, absorbtion + emmission
//    front to back (volume): normal volume rendering
//  dynamic(scattering):
//    domain passing - 
//      front to back: volume rendering and ray tracing
//      back to front: both energy types.
//      
//
//
namespace rover {

template<typename FloatType>
class Scheduler {
public:
  Scheduler();
  virtual ~Scheduler();
  virtual void trace_rays();
  void save_result(std::string file_name);
  void clear_data_sets();
  //
  // Setters
  //
  virtual void set_render_settings(const RenderSettings render_settings);
  virtual void add_data_set(vtkmDataSet &data_set);
  virtual void set_ray_generator(RayGenerator<FloatType> *ray_generator);
  //
  // Getters
  //

  RenderSettings get_render_settings() const;
  vtkmDataSet    get_data_set(const int &domain);
  FloatType *    get_color_buffer();
protected:
  std::vector<Domain>                       m_domains;
  PartialComposite<FloatType>               m_result;
  std::vector<PartialComposite<FloatType>>  m_partial_composites;
  RenderSettings                            m_render_settings;
  RayGenerator<FloatType>                  *m_ray_generator;
private:

};

}; // namespace rover
#endif 
