#ifndef rover_h
#define rover_h

#include <image.hpp>
#include <rover_exports.h>
#include <rover_types.hpp>
#include <ray_generators/ray_generator.hpp>
// vtk-m includes
#include <vtkm_typedefs.hpp>

// std includes
#include <memory>

#ifdef PARALLEL
#include <mpi.h>
#endif

namespace rover {

class Rover 
{
public:
  Rover();
  ~Rover();
#ifdef PARALLEL
  void init(MPI_Comm comm_handle);
#else
  void init();
#endif
  void finalize();

  void add_data_set(vtkmDataSet &);
  void set_render_settings(const RenderSettings render_settings);
  void set_ray_generator(RayGenerator *);
  void clear_data_sets();
  void set_background(const std::vector<vtkm::Float32> &background);
  void set_background(const std::vector<vtkm::Float64> &background);
  void execute();
  void about();
  void save_png(const std::string &file_name);
  void set_tracer_precision32();
  void set_tracer_precision64();
  void get_result(Image<vtkm::Float32> &image);
  void get_result(Image<vtkm::Float64> &image);
private:
  class InternalsType;
  std::shared_ptr<InternalsType> m_internals; 
}; // class strawman 

}; // namespace rover

#endif
