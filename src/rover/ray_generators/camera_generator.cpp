#include <ray_generators/camera_generator.hpp>
namespace rover {

template<typename Precision>
CameraGenerator<Precision>::CameraGenerator()
 : RayGenerator<Precision>()
{

}

template<typename Precision>
CameraGenerator<Precision>::CameraGenerator(const vtkmCamera &camera,
                                            vtkmCoordinates coordinates)
 : RayGenerator<Precision>()
{
  m_camera = camera;
  this->m_height = m_camera.GetHeight();
  this->m_width  = m_camera.GetWidth();
  m_coordinates = coordinates;
}

template<typename Precision>
CameraGenerator<Precision>::~CameraGenerator()
{

}

template<typename Precision>
vtkmRayTracing::Ray<Precision> 
CameraGenerator<Precision>::get_rays() 
{
  m_camera.CreateRays(this->m_rays, this->m_coordinates); 
  this->m_has_rays = false;
  if(this->m_rays.NumRays == 0) std::cout<<"CameraGenerator Warning no rays were generated\n";
  return this->m_rays;
}

template<typename Precision>
vtkmCamera 
CameraGenerator<Precision>::get_camera()
{
  return m_camera;
}

template<typename Precision>
vtkmCoordinates 
CameraGenerator<Precision>::get_coordinates()
{
  return m_coordinates;
}

template class CameraGenerator<vtkm::Float32>;
template class CameraGenerator<vtkm::Float64>;

} // namespace rover
