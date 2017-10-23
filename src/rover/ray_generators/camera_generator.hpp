#ifndef rover_camera_generator_h
#define rover_camera_generator_h

#include <ray_generators/ray_generator.hpp>

namespace rover {

class CameraGenerator : public RayGenerator
{
public:
  CameraGenerator(const vtkmCamera &camera);
  virtual ~CameraGenerator();
  virtual void get_rays(vtkmRayTracing::Ray<vtkm::Float32> &rays);
  virtual void get_rays(vtkmRayTracing::Ray<vtkm::Float64> &rays);
  vtkmCamera get_camera();
  vtkmCoordinates get_coordinates();
  void set_coordinates(vtkmCoordinates coordinates);
protected:
  CameraGenerator(); 
  vtkmCoordinates m_coordinates;
  vtkmCamera m_camera;
};

} // namespace rover
#endif
