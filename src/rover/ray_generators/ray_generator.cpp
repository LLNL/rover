#include <ray_generators/ray_generator.hpp>
namespace rover {

template<typename Precision>
RayGenerator<Precision>::RayGenerator(vtkmRayTracing::Ray<Precision> rays,
                                      int height,
                                      int width)
{
  m_rays = rays;
  m_height = height;
  m_width = width;
  m_has_rays = true;
}

template<typename Precision>
RayGenerator<Precision>::RayGenerator()
  : m_height(-1), 
    m_width(-1)
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
  m_has_rays = false;
}

template<typename Precision>
void
RayGenerator<Precision>::get_dims(int &height, int &width) const
{
  height = m_height;
  width  = m_width;
}

template<typename Precision>
void
RayGenerator<Precision>::set_rays(vtkmRayTracing::Ray<Precision> rays,
                                  int height,
                                  int width)
{
  m_rays = rays;
  m_height = height;
  m_width = width;
  m_has_rays = true;
}

template<typename Precision>
bool RayGenerator<Precision>::get_has_rays() const
{
  return m_has_rays; 
}

template<typename Precision>
int RayGenerator<Precision>::get_size() const
{
  return m_height * m_width; 
}
template class RayGenerator<vtkm::Float32>;
template class RayGenerator<vtkm::Float64>;

} // naspace rover
