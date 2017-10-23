#ifndef rover_compositor_h
#define rover_compositor_h
#include <rover_types.hpp>


#include <compositing/absorption_partial.hpp>
#include <compositing/emission_partial.hpp>
#include <compositing/volume_partial.hpp>

#ifdef PARALLEL
#include <mpi.h>
#endif

namespace rover {

template<typename PartialType> 
class Compositor
{
public:
  Compositor();
  ~Compositor();
  PartialImage<typename PartialType::ValueType> 
  composite(std::vector<PartialImage<typename PartialType::ValueType>> &partial_images);
  void set_background(std::vector<vtkm::Float32> &background_values);
  void set_background(std::vector<vtkm::Float64> &background_values);
#ifdef PARALLEL
  void set_comm_handle(MPI_Comm comm_hanlde);
#endif
protected:
  void extract(std::vector<PartialImage<typename PartialType::ValueType>> &partial_images, 
               std::vector<PartialType> &partials,
               int &global_min_pixel,
               int &global_max_pixel);

  void composite_partials(std::vector<PartialType> &partials, 
                          std::vector<PartialType> &output_partials);

  std::vector<typename PartialType::ValueType> m_background_values;
#ifdef PARALLEL
  MPI_Comm m_comm_handle;
#endif
};

}; // namespace rover
#endif
