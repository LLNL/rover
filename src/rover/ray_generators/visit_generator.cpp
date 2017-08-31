#include <ray_generators/visit_generator.hpp>
#include <vtkm/VectorAnalysis.h>
#include <limits>
namespace rover {

template<typename Precision>
VisitGenerator<Precision>::VisitGenerator()
 : RayGenerator<Precision>()
{

}

template<typename Precision>
VisitGenerator<Precision>::VisitGenerator(const VisitParams &params)
 : RayGenerator<Precision>()
{
  m_params = params;
}

template<typename Precision>
VisitGenerator<Precision>::~VisitGenerator()
{

}

template<typename Precision>
vtkmRayTracing::Ray<Precision> 
VisitGenerator<Precision>::get_rays() 
{
  const int size = m_params.m_image_dims[0] * m_params.m_image_dims[1];
  vtkmRayTracing::Ray<Precision> rays(size, vtkm::cont::DeviceAdapterTagSerial());
  
  vtkm::Vec<Precision,3> view_side;

  view_side[0] = m_params.m_view_up[1] * m_params.m_normal[2] 
                 - m_params.m_view_up[2] * m_params.m_normal[1];

  view_side[1] = -m_params.m_view_up[0] * m_params.m_normal[2] 
                 + m_params.m_view_up[2] * m_params.m_normal[0];

  view_side[2] = m_params.m_view_up[0] * m_params.m_normal[1] 
                 - m_params.m_view_up[1] * m_params.m_normal[0];

  Precision near_height, view_height, far_height;
  Precision near_width, view_width, far_width;;

  view_height = m_params.m_parallel_scale;
  // I think this is flipped
  view_width = view_height * (m_params.m_image_dims[1] / m_params.m_image_dims[0]);
  if(m_params.m_perspective)
  {
    Precision view_dist = m_params.m_parallel_scale / tan((m_params.m_view_angle * 3.1415926535) / 360.);
    Precision near_dist = view_dist + m_params.m_near_plane;
    Precision far_dist  = view_dist + m_params.m_far_plane;
    near_height = (near_dist * view_height) / view_dist;
    near_width  = (near_dist * view_width) / view_dist;
    far_height  = (far_dist * view_height) / view_dist;
    far_width   = (far_dist * view_width) / view_dist;
  }
  else
  {
    near_height = view_height;
    near_width  = view_width;
    far_height  = view_height;
    far_width   = view_width;
  }

  near_height = near_height / m_params.m_image_zoom;
  near_width  = near_width  / m_params.m_image_zoom;
  far_height  = far_height  / m_params.m_image_zoom;
  far_width   = far_width   / m_params.m_image_zoom;

  vtkm::Vec<Precision,3> near_origin;
  vtkm::Vec<Precision,3> far_origin;
  near_origin = m_params.m_focus + m_params.m_near_plane * m_params.m_normal;
  far_origin = m_params.m_focus + m_params.m_far_plane * m_params.m_normal;

  Precision near_dx, near_dy, far_dx, far_dy;
  near_dx = (2. * near_width)  / m_params.m_image_dims[0];
  near_dy = (2. * near_height) / m_params.m_image_dims[1];
  far_dx  = (2. * far_width)   / m_params.m_image_dims[0];
  far_dy  = (2. * far_height)  / m_params.m_image_dims[1];

  auto origin_x = rays.OriginX.GetPortalControl(); 
  auto origin_y = rays.OriginY.GetPortalControl(); 
  auto origin_z = rays.OriginZ.GetPortalControl(); 

  auto dir_x = rays.DirX.GetPortalControl(); 
  auto dir_y = rays.DirY.GetPortalControl(); 
  auto dir_z = rays.DirZ.GetPortalControl(); 

  auto pixel_id = rays.PixelIdx.GetPortalControl(); 
  const int x_size = m_params.m_image_dims[0]; 
  const int y_size = m_params.m_image_dims[1]; 

  const Precision x_factor = - (2. * m_params.m_image_pan[0] * m_params.m_image_zoom + 1.);
  const Precision x_start  = x_factor * near_width + near_dx / 2.;
  const Precision x_end    = x_factor * far_width + far_dx / 2.;

  const Precision y_factor = - (2. * m_params.m_image_pan[1] * m_params.m_image_zoom + 1.);
  const Precision y_start  = y_factor * near_height + near_dy / 2.;
  const Precision y_end    = y_factor * far_height + far_dy / 2.;

  for(int y = 0; y < y_size; ++y)
  {
    const Precision near_y = y_start + Precision(y) * near_dy;
    const Precision far_y = y_end + Precision(y) * far_dy;
    #pragma omp parallel for
    for(int x = 0; x < x_size; ++x)
    {
      const int id = y * x_size + x;    

      Precision near_x = x_start + Precision(x) * near_dx;
      Precision far_x = x_end + Precision(x) * far_dx;

      vtkm::Vec<Precision,3> start;
      vtkm::Vec<Precision,3> end;
      start = near_origin + near_x * view_side + near_y * m_params.m_view_up;
      end = far_origin + far_x * view_side + far_y * m_params.m_view_up;

      vtkm::Vec<Precision,3> dir = end - start;
      vtkm::Normalize(dir);

      pixel_id.Set(id, id);
      origin_x.Set(id, start[0]);
      origin_y.Set(id, start[1]);
      origin_z.Set(id, start[2]);

      dir_x.Set(id, dir[0]);
      dir_y.Set(id, dir[1]);
      dir_z.Set(id, dir[2]);
      
    }
  }
  // set a couple other ray variables
  #pragma omp parallel for
  for(int i = 0; i < size; ++i)
  {
    rays.HitIdx.GetPortalControl().Set(i, -2);
    rays.MinDistance.GetPortalControl().Set(i, 0.f);
    rays.MaxDistance.GetPortalControl().Set(i, std::numeric_limits<Precision>::max());
  }
  
  this->m_width  = m_params.m_image_dims[0];
  this->m_height = m_params.m_image_dims[1];
  return rays;
}

template<typename Precision>
void
VisitGenerator<Precision>::set_params(const VisitParams &params)
{
  m_params = params;
}

template class VisitGenerator<vtkm::Float32>;
template class VisitGenerator<vtkm::Float64>;

} // namespace rover
