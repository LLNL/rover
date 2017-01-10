#ifndef rover_ray_generator_h
#define rover_ray_generator_h
#include <vtkm_typedefs.hpp>
namespace rover {

template<typename FloatType>
class RayGenerator 
{
public:
  typename typedef FloatType Precision;
  RayGenerator(vtkmRayTraceing::Ray<Precision> rays); 
  virtual ~RayGenerator();

  virtual vtkmRayTracing::Ray<Precision> get_rays();
protected:
  RayGenerator();
  vtkmRayTracing::Ray<Precision> m_rays; 
};
}; //namespace rover
#endif
