#include <schedular.hpp>
#include <rover.hpp>
#include <vtkm_typedefs.hpp>
#include <iostream>
namespace rover {
template<typename FloatType>
class Rover<FloatType>::InternalsType 
{
protected:
  Schedular<FloatType>      *m_schedular;
  void reset_render_mode(RenderMode render_mode)
  {
  }

public: 
  InternalsType()
  {
#ifdef PARALLEL
    m_schedular = new StaticSchedular<FloatType>();
#else
    m_schedular = new Schedular<FloatType>();
#endif
  }

  void set_data_set(vtkmDataSet &dataset)
  {
    m_schedular->set_data_set(dataset);
  }

  void set_render_settings(RenderSettings render_settings)
  {
#ifdef PARALLEL
    // logic to create the appropriate parallel schedular
    //
    // ray tracing = dynamic schedular, scattering | no_scattering
    // volume/engery = scattering + local_scope -> dynamic schedular 
    //                 non_scattering + global_scope ->static schedular
    //
    // Note: I wanted to allow for the case of scattering + global scope. This could 
    //       be benificial in the case where we may or may not scatter in a given 
    //       domain. Thus, avoid waiting for the ray to emerge or throw out the results
#else
      if(m_schedular == NULL) delete m_schedular;
      m_schedular = new Schedular<FloatType>();
#endif
   }

  ~InternalsType()
  {
    if(m_schedular) delete m_schedular;
  }

};

template<typename FloatType>
Rover<FloatType>::Rover()
  : m_internals( new InternalsType )
{

}

template<typename FloatType>
Rover<FloatType>::~Rover()
{
  
}

template<typename FloatType>
void
Rover<FloatType>::init()
{
  // initialize
}

template<typename FloatType>
void
Rover<FloatType>::finalize()
{
  // finalize
}

template<typename FloatType>
void
Rover<FloatType>::set_data_set(vtkmDataSet &dataset)
{
  m_internals->set_data_set(dataset); 
}

template<typename FloatType>
void
Rover<FloatType>::set_render_settings(RenderSettings render_settings)
{
  m_internals->set_render_settings(render_settings);
}

// Explicit instantiations
template class Rover<vtkm::Float32>; 
template class Rover<vtkm::Float64>; 

}; //namespace rover

