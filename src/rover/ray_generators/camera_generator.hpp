#ifndef rover_camera_generator_h
#define rover_camera_generator_h

#include <ray_generators/ray_generator.hpp>

namespace rover {

template<typename Precision>
class CameraGenerator : public RayGenerator<Precision>
{
public:
  CameraGenerator(const vtkmCamera &camera);
  virtual ~CameraGenerator();
  virtual vtkmRayTracing::Ray<Precision> get_rays(); 
  vtkmCamera get_camera();
  vtkmCoordinates get_coordinates();
  void set_coordinates(vtkmCoordinates coordinates);
protected:
  CameraGenerator(); 
  vtkmCoordinates m_coordinates;
  vtkmCamera m_camera;
};

typedef CameraGenerator<vtkm::Float32> CameraGenerator32;
typedef CameraGenerator<vtkm::Float64> CameraGenerator64;
} // namespace rover
#endif
