#ifndef rover_camera_generator_h
#define rover_camera_generator_h

#include <ray_generators/ray_generator.hpp>

namespace rover {

template<typename Precision>
class CameraGenerator : public RayGenerator<Precision>
{
public:
  CameraGenerator(const vtkmCamera &camera, vtkmCoordinates coordinates);
  virtual ~CameraGenerator();
  virtual vtkmRayTracing::Ray<Precision> get_rays(); 
  vtkmCamera &get_camera();
  vtkmCoordinates get_coordinates();
protected:
  CameraGenerator(); 
  vtkmCoordinates m_coordinates;
  vtkmCamera m_camera;
};

} // namespace rover
#endif
