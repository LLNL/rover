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

template<typename FloatType>
class Rover 
{
public:
  Rover();
  ~Rover();
  typedef FloatType Precision;
#ifdef PARALLEL
  void init(MPI_Comm comm_handle);
#else
  void init();
#endif
  void finalize();

  void add_data_set(vtkmDataSet &);
  void set_render_settings(const RenderSettings render_settings);
  void set_ray_generator(RayGenerator<FloatType> *);
  void clear_data_sets();
  void set_background(const std::vector<FloatType> &background);
  void execute();
  void about();
  void save_png(const std::string &file_name);
  Image<FloatType> get_result();
private:
  class InternalsType;
  std::shared_ptr<InternalsType> m_internals; 
}; // class strawman 

typedef Rover<float>  Rover32;
typedef Rover<double> Rover64; 
}; // namespace rover

#endif
