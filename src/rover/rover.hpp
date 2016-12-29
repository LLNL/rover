#ifndef rover_h
#define rover_h

#include <rover_exports.h>

// vtk-m includes
#include <vtkm/cont/DataSet.h>

// std includes
#include <memory>

namespace rover {

enum Promise { contiguous_domains, non_contiguous_domains };
// this could be ray tracing / volume rendering / energy 
enum RenderMode { volume, energy, surface };


class Rover 
{
public:
  Rover();
  ~Rover();
  
  void init();
  void finalize();

  void set_dataset(vtkm::cont::DataSet &);
  void set_render_mode(RenderMode render_mode);
  void set_promise(Promise promise);
private:
  class InternalsType
  std::shared_ptr<InternalsType> m_internals; 
}; // class strawman 

}; // namespace rover

#endif
