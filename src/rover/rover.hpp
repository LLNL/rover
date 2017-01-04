#ifndef rover_h
#define rover_h

#include <rover_exports.h>
#include <rover_types.hpp>
// vtk-m includes
#include <vtkm_typedefs.hpp>

// std includes
#include <memory>

namespace rover {

class Rover 
{
public:
  Rover();
  ~Rover();

  void init();
  void finalize();

  void set_data_set(vtkmDataSet &);
  void set_render_settings(const RenderSettings render_settings);
private:
  class InternalsType;
  std::shared_ptr<InternalsType> m_internals; 
}; // class strawman 

}; // namespace rover

#endif
