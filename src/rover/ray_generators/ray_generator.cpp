#include <ray_generators/ray_generator.hpp>
namespace rover {

RayGenerator::RayGenerator(int height,
                           int width)
{
  m_height = height;
  m_width = width;
  m_has_rays = true;
}

RayGenerator::RayGenerator()
  : m_height(-1), 
    m_width(-1)
{
}

RayGenerator::~RayGenerator()
{

}

void
RayGenerator::get_dims(int &height, int &width) const
{
  height = m_height;
  width  = m_width;
}

bool 
RayGenerator::get_has_rays() const
{
  return m_has_rays; 
}

void 
RayGenerator::reset() 
{
  m_has_rays = true;
}

int 
RayGenerator::get_size() const
{
  return m_height * m_width; 
}

} // naspace rover
