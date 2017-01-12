#ifndef rover_ray_generator_h
#define rover_ray_generator_h
#include <vtkm_typedefs.hpp>
namespace rover {

template<typename FloatType>
class RayGenerator 
{
public:
  typedef FloatType Precision;
  RayGenerator();
  RayGenerator(vtkmRayTracing::Ray<Precision> rays); 
  virtual ~RayGenerator(); 
  virtual vtkmRayTracing::Ray<Precision> get_rays();
  virtual void set_rays(vtkmRayTracing::Ray<Precision> rays);
protected:
  vtkmRayTracing::Ray<Precision> m_rays; 
};
}; //namespace rover
#endif
