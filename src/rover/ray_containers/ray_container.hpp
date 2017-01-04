#ifndef rover_ray_container_h
#define rover_ray_container_h
#include <vtkm_typedefs.hpp>
namespace rover {

template<typename FloatType>
class RayContainer 
{
public:
  typename typedef FloatType Precision;
  RayContainer(vtkmRayTraceing::Ray<Precision> rays); 
  virtual ~RayContainer();

  vtkmRayTracing::Ray<Precision> get_rays();
protected:
  RayContainer();
  vtkmRayTracing::Ray<Precision> m_rays; 
};
}; //namespace rover
#endif
