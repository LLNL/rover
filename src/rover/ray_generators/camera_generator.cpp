#include <ray_generators/camera_generator.hpp>
namespace rover {

template<typename Precision>
CameraGenerator<Precision>::CameraGenerator()
 : RayGenerator<Precision>()
{

}

template<typename Precision>
CameraGenerator<Precision>::CameraGenerator(const vtkmCamera &camera)
 : RayGenerator<Precision>()
{
  m_camera = camera;
  this->m_height = m_camera.GetHeight();
  this->m_width  = m_camera.GetWidth();
}

template<typename Precision>
CameraGenerator<Precision>::~CameraGenerator()
{

}

template<typename Precision>
vtkmRayTracing::Ray<Precision> 
CameraGenerator<Precision>::get_rays() 
{
  vtkmRayTracing::Ray<Precision> rays;
  m_camera.CreateRays(rays, this->m_coordinates); 
  //vtkm::Vec<vtkm::Int32,2> pixel(250,413);
  //m_camera.CreateDebugRay(pixel,rays); 
  this->m_has_rays = false;
  if(rays.NumRays == 0) std::cout<<"CameraGenerator Warning no rays were generated\n";
  //rays.DebugWidth =  m_camera.GetSubsetWidth();
  //rays.DebugHeight = m_camera.GetSubsetHeight();
  return rays;
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

template<typename Precision>
void
CameraGenerator<Precision>::set_coordinates(vtkmCoordinates coordinates)
{
  m_coordinates = coordinates;
}

template class CameraGenerator<vtkm::Float32>;
template class CameraGenerator<vtkm::Float64>;

} // namespace rover
