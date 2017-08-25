#ifndef rover_visit_generator_h
#define rover_visit_generator_h

#include <ray_generators/ray_generator.hpp>

namespace rover {

template<typename Precision>
class VisitGenerator : public RayGenerator<Precision>
{
public:
  struct VisitParams
  {
    vtkm::Vec<Precision,3> m_normal;
    vtkm::Vec<Precision,3> m_focus;
    vtkm::Vec<Precision,3> m_view_up;

    vtkm::Vec<Precision,2> m_image_pan;
    vtkm::Vec<int,2>       m_image_dims;

    Precision              m_view_angle;
    Precision              m_parallel_scale;
    Precision              m_near_plane;
    Precision              m_far_plane;
    Precision              m_image_zoom;
    bool                   m_perspective;

    VisitParams()
      : m_normal(0.f, 0.f, 0.f),
        m_focus(0.f, 0.f, 0.f),
        m_view_up(0.f, 1.f, 0.f),
        m_image_pan(0.f, 0.f),
        m_image_dims(500, 500),
        m_view_angle(30.f),
        m_parallel_scale(.5f),
        m_near_plane(-0.5f),
        m_far_plane(0.5f),
        m_image_zoom(1.f),
        m_perspective(true)
    { }

  };

  VisitGenerator(const VisitParams &params);
  virtual ~VisitGenerator();
  virtual vtkmRayTracing::Ray<Precision> get_rays(); 
  
  void set_params(const VisitParams &params);
protected:
  VisitGenerator(); 
  VisitParams m_params;
};

typedef VisitGenerator<vtkm::Float32> VisitGenerator32;
typedef VisitGenerator<vtkm::Float64> VisitGenerator64;
} // namespace rover
#endif