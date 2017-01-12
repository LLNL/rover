#include <ray_generators/ray_generator.hpp>
namespace rover {

template<typename Precision>
RayGenerator<Precision>::RayGenerator(vtkmRayTracing::Ray<Precision> rays)
{
  m_rays = rays;
}

template<typename Precision>
RayGenerator<Precision>::RayGenerator()
{
}

template<typename Precision>
RayGenerator<Precision>::~RayGenerator()
{

}

template<typename Precision>
vtkmRayTracing::Ray<Precision> 
RayGenerator<Precision>::get_rays()
{
  return m_rays;
}

template<typename Precision>
void
RayGenerator<Precision>::set_rays(vtkmRayTracing::Ray<Precision> rays)
{
  m_rays = rays;
}

template class RayGenerator<vtkm::Float32>;
template class RayGenerator<vtkm::Float64>;

} // naspace rover
