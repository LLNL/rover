#ifndef rover_scheduler_h
#define rover_scheduler_h

#include <domain.hpp>
#include <image.hpp>
#include <engine.hpp>
#include <scheduler_base.hpp>
#include <rover_types.hpp>
#include <ray_generators/ray_generator.hpp>
#include <vtkm_typedefs.hpp>

#ifdef PARALLEL
#include <mpi.h>
#endif
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
class Scheduler : public SchedulerBase
{
public:
  Scheduler();
  virtual ~Scheduler();
  void trace_rays() override;
  void save_result(std::string file_name) override;

  virtual void get_result(Image<vtkm::Float32> &image);
  virtual void get_result(Image<vtkm::Float64> &image);
protected:
  void set_global_scalar_range();
  void set_global_bounds();
  int  get_global_channels();
  Image<FloatType>                          m_result;
  std::vector<PartialImage<FloatType>>      m_partial_images;
private:

};

}; // namespace rover
#endif 
