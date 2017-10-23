#ifndef rover_ray_generator_h
#define rover_ray_generator_h
#include <vtkm_typedefs.hpp>
namespace rover {

class RayGenerator 
{
public:
  RayGenerator();
  RayGenerator(int height, int width); 
  virtual ~RayGenerator(); 
  virtual void get_rays(vtkmRayTracing::Ray<vtkm::Float32> &rays) = 0;
  virtual void get_rays(vtkmRayTracing::Ray<vtkm::Float64> &rays) = 0;
  //virtual void set_rays(vtkmRayTracing::Ray<Precision> rays, int height = -1, int width = -1);
  void get_dims(int &height, int &width) const;
  int  get_size() const;
  bool get_has_rays() const;
  void reset();
protected:
  int  m_height;
  int  m_width;
  bool m_has_rays;
};
}; //namespace rover
#endif
