#ifndef rover_compositor_h
#define rover_compositor_h
#include <rover_types.hpp>

#include <compositing/volume_block.hpp>
#include <compositing/absorption_block.hpp>

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
  template<typename FloatType> 
  PartialImage<FloatType> composite(std::vector<PartialImage<FloatType>> &partial_images);
#ifdef PARALLEL
  void set_comm_handle(MPI_Comm comm_hanlde);
#endif
protected:
  template<typename FloatType>
  void extract(std::vector<PartialImage<FloatType>> &partial_images, 
               std::vector<PartialType> &partials,
               int &global_min_pixel,
               int &global_max_pixel);
  void composite_partials(std::vector<PartialType> &partials, 
                          std::vector<PartialType> &output_partials);

  std::vector<double> m_background_values;
#ifdef PARALLEL
  MPI_Comm m_comm_handle;
#endif
};

}; // namespace rover
#endif
