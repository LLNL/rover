#include <ray_generator.hpp>
namespace rover {

RayGenerator::RayGenerator(vtkmRayTraceing::Ray<Precision> rays)
{

}

RayGenerator::~RayGenerator()
{

}

vtkmRayTracing::Ray<Precision> 
RayGenerator::get_rays()
{
  return m_rays;
}

} // naspace rover
