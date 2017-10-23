#include <ray_generators/camera_generator.hpp>
namespace rover {

CameraGenerator::CameraGenerator()
 : RayGenerator()
{

}

CameraGenerator::CameraGenerator(const vtkmCamera &camera)
 : RayGenerator()
{
  m_camera = camera;
  this->m_height = m_camera.GetHeight();
  this->m_width  = m_camera.GetWidth();
}

CameraGenerator::~CameraGenerator()
{

}

void 
CameraGenerator::get_rays(vtkmRayTracing::Ray<vtkm::Float32> &rays) 
{
  m_camera.CreateRays(rays, this->m_coordinates); 
  this->m_has_rays = false;
  if(rays.NumRays == 0) std::cout<<"CameraGenerator Warning no rays were generated\n";
}

void
CameraGenerator::get_rays(vtkmRayTracing::Ray<vtkm::Float64> &rays) 
{
  m_camera.CreateRays(rays, this->m_coordinates); 
  this->m_has_rays = false;
  if(rays.NumRays == 0) std::cout<<"CameraGenerator Warning no rays were generated\n";
}

vtkmCamera 
CameraGenerator::get_camera()
{
  return m_camera;
}

vtkmCoordinates 
CameraGenerator::get_coordinates()
{
  return m_coordinates;
}

void
CameraGenerator::set_coordinates(vtkmCoordinates coordinates)
{
  m_coordinates = coordinates;
}

} // namespace rover
