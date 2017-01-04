#include <schedular.hpp>
#include <volume_engine.hpp>
namespace rover {

template<typename FloatType>
Schedular<FloatType>::Schedular()
{
  m_engine = NULL;
}

template<typename FloatType>
Schedular<FloatType>::~Schedular()
{
  if(m_engine) delete m_engine;
}

template<typename FloatType>
void
Schedular<FloatType>::set_render_settings(const RenderSettings render_render_settings)
{
  //
  //  In the serial schedular, the only setting that matter are 
  //  m_render_mode and m_scattering_mode
  //

  // 
  // Create the correct engine
  //
  if(m_engine) delete m_engine;

  if(m_render_settings.m_render_mode == volume)
  {
    m_engine = new VolumeEngine(); 
  }
  else if(m_render_settings.m_render_mode == energy)
  {
    std::cout<<"engergy not implemented\n";
  }
  else if(m_render_settings.m_render_mode == surface)
  {
    std::cout<<"ray tracing not implemented\n";
  }
}
template<typename FloatType>
void 
Schedular<FloatType>::set_data_set(vtkmDataSet &dataset)
{
  m_engine->set_data_set(dataset);
}

template<typename FloatType>
vtkmDataSet
Schedular<FloatType>::get_data_set() const
{
  return m_data_set;
}

template<typename FloatType>
RenderSettings
Schedular<FloatType>::get_render_settings() const
{
  return m_render_settings;
}

//
// in the other schedulars this method will be far from trivial
//
template<typename FloatType>
void 
Schedular<FloatType>::trace_rays(vtkm::rendering::raytracing::Ray<FloatType> &rays)
{
  m_engine->trace(rays);
}

// Explicit instantiation
template class Schedular<vtkm::Float32>;
template class Schedular<vtkm::Float64>;
}; // namespace rover
