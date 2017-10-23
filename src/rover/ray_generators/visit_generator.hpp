#ifndef rover_visit_generator_h
#define rover_visit_generator_h

#include <ray_generators/ray_generator.hpp>

namespace rover {

class VisitGenerator : public RayGenerator
{
public:
  struct VisitParams
  {
    vtkm::Vec<double,3> m_normal;
    vtkm::Vec<double,3> m_focus;
    vtkm::Vec<double,3> m_view_up;

    vtkm::Vec<double,2> m_image_pan;
    vtkm::Vec<int,2>       m_image_dims;

    double                 m_view_angle;
    double                 m_parallel_scale;
    double                 m_near_plane;
    double                 m_far_plane;
    double                 m_image_zoom;
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

    void print() const
    {
      std::cout<<"******** VisIt Parmas *********\n";
      std::cout<<"normal        : "<<m_normal<<"\n";
      std::cout<<"focus         : "<<m_focus<<"\n";
      std::cout<<"up            : "<<m_view_up<<"\n";
      std::cout<<"pan           : "<<m_image_pan<<"\n";
      std::cout<<"dims          : "<<m_image_dims<<"\n";
      std::cout<<"view angle    : "<<m_image_dims<<"\n";
      std::cout<<"parallel scale: "<<m_parallel_scale<<"\n";
      std::cout<<"near_plane    : "<<m_near_plane<<"\n";
      std::cout<<"far_plane     : "<<m_far_plane<<"\n";
      std::cout<<"zoom          : "<<m_image_zoom<<"\n";
      std::cout<<"perspective   : "<<m_perspective<<"\n";
    }


  };

  VisitGenerator(const VisitParams &params);
  virtual ~VisitGenerator();

  virtual void get_rays(vtkmRayTracing::Ray<vtkm::Float32> &rays);
  virtual void get_rays(vtkmRayTracing::Ray<vtkm::Float64> &rays);
  
  void set_params(const VisitParams &params);
  void print_params() const;
protected:
  VisitGenerator(); 
  VisitParams m_params;
  template<typename T> void gen_rays(vtkmRayTracing::Ray<T> &rays);
};

} // namespace rover
#endif
