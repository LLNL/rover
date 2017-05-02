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
  RayGenerator(vtkmRayTracing::Ray<Precision> rays, int height = -1, int width = -1); 
  virtual ~RayGenerator(); 
  virtual vtkmRayTracing::Ray<Precision> get_rays();
  virtual void set_rays(vtkmRayTracing::Ray<Precision> rays, int height = -1, int width = -1);
  void get_dims(int &height, int &width) const;
  int  get_size() const;
  bool get_has_rays() const;
  void reset();
protected:
  vtkmRayTracing::Ray<Precision> m_rays; 
  int  m_height;
  int  m_width;
  bool m_has_rays;
};
}; //namespace rover
#endif
