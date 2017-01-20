#ifndef rover_h
#define rover_h

#include <rover_exports.h>
#include <rover_types.hpp>
#include <ray_generators/ray_generator.hpp>
// vtk-m includes
#include <vtkm_typedefs.hpp>

// std includes
#include <memory>

namespace rover {

template<typename FloatType>
class Rover 
{
public:
  Rover();
  ~Rover();

  void init();
  void finalize();

  void set_data_set(vtkmDataSet &);
  void set_render_settings(const RenderSettings render_settings);
  void set_ray_generator(RayGenerator<FloatType> *);
  void execute();
  void about();
  void save_png(const std::string &file_name);
private:
  class InternalsType;
  std::shared_ptr<InternalsType> m_internals; 
}; // class strawman 

typedef Rover<float>  Rover32;
typedef Rover<double> Rover64; 
}; // namespace rover

#endif
