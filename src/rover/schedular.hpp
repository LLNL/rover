#ifndef rover_schedular_h
#define rover_schedular_h
#include <engine.hpp>
#include <rover_types.hpp>
#include <ray_generators/ray_generator.hpp>
#include <vtkm_typedefs.hpp>
//
// Schedular types:
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
class Schedular {
public:
  Schedular();
  virtual ~Schedular();
  virtual void trace_rays();
  //
  // Setters
  //
  virtual void set_render_settings(const RenderSettings render_settings);
  virtual void set_data_set(vtkmDataSet &data_set);
  virtual void set_ray_generator(RayGenerator<FloatType> *ray_generator);
  //
  // Getters
  //
  RenderSettings get_render_settings() const;
  vtkmDataSet    get_data_set() const;
  float* get_color_buffer();
protected:
  Engine                    *m_engine;
  vtkmDataSet                m_data_set;
  RenderSettings             m_render_settings;
  RayGenerator<FloatType>   *m_ray_generator;
  vtkmColorBuffer            m_color_buffer;
private:

};

}; // namespace rover
#endif 
