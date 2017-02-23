#ifndef rover_compositor_h
#define rover_compositor_h
#include <rover_types.hpp>

#ifdef PARALLEL
#include <mpi.h>
#endif

namespace rover {

class VolumeCompositor
{
public:
  VolumeCompositor();
  ~VolumeCompositor();
  template<typename FloatType> 
  PartialImage<FloatType> composite(std::vector<PartialImage<FloatType>> &partial_images);
#ifdef PARALLEL
  void set_comm_handle(MPI_Comm comm_hanlde);
#endif
protected:
  struct PartialComposite;  
#ifdef PARALLEL
  MPI_Comm m_comm_handle;
#endif
};

}; // namespace rover
#endif
