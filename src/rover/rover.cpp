#include <scheduler.hpp>
#include <rover.hpp>
#include <rover_exceptions.hpp>
#include <vtkm_typedefs.hpp>
#include <iostream>
#include <utils/rover_logging.hpp>
namespace rover {
template<typename FloatType>
class Rover<FloatType>::InternalsType 
{
protected:
  Scheduler<FloatType>      *m_scheduler;
  void reset_render_mode(RenderMode render_mode)
  {
  }

public: 
  InternalsType()
  {
#ifdef PARALLEL
    m_scheduler = new StaticScheduler<FloatType>();
#else
    m_scheduler = new Scheduler<FloatType>();
#endif
  }

  void add_data_set(vtkmDataSet &dataset)
  {
    m_scheduler->add_data_set(dataset);
  }

  void set_render_settings(RenderSettings render_settings)
  {
    ROVER_INFO("set_render_settings");
    // TODO: make copy constructors to get the members like ray_generator
#ifdef PARALLEL
    // logic to create the appropriate parallel scheduler
    //
    // ray tracing = dynamic scheduler, scattering | no_scattering
    // volume/engery = scattering + local_scope -> dynamic scheduler 
    //                 non_scattering + global_scope ->static scheduler
    //
    // Note: I wanted to allow for the case of scattering + global scope. This could 
    //       be benificial in the case where we may or may not scatter in a given 
    //       domain. Thus, avoid waiting for the ray to emerge or throw out the results
#else
      if(m_scheduler == NULL) delete m_scheduler;
      m_scheduler = new Scheduler<FloatType>();
      m_scheduler->set_render_settings(render_settings);
#endif
   }

  void set_ray_generator(RayGenerator<FloatType> *ray_generator)
  {
    m_scheduler->set_ray_generator(ray_generator); 
  }

  void clear_data_sets()
  {
    m_scheduler->clear_data_sets(); 
  }

  ~InternalsType()
  {
    if(m_scheduler) delete m_scheduler;
  }

  void save_png(const std::string &file_name)
  {
    m_scheduler->save_result(file_name);
  }

  void execute()
  {
    m_scheduler->trace_rays();
  }

}; //Internals Type

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
Rover<FloatType>::add_data_set(vtkmDataSet &dataset)
{
  m_internals->add_data_set(dataset); 
}

template<typename FloatType>
void
Rover<FloatType>::set_render_settings(RenderSettings render_settings)
{
  m_internals->set_render_settings(render_settings);
}

template<typename FloatType>
void
Rover<FloatType>::clear_data_sets()
{
  m_internals->clear_data_sets();
}

template<typename FloatType>
void
Rover<FloatType>::set_ray_generator(RayGenerator<FloatType> *ray_generator)
{
  if(ray_generator == nullptr)
  {
    throw RoverException("Ray generator cannot  be null");    
  }
  m_internals->set_ray_generator(ray_generator);
}

template<typename FloatType>
void
Rover<FloatType>::execute()
{
  m_internals->execute(); 
}

template<typename T> 
void 
print_type(T ) { } 

template<> 
void 
print_type<vtkm::Float32>(vtkm::Float32 )
{
  std::cout<<"Single precision\n";
}

template<> 
void
print_type<vtkm::Float64>(vtkm::Float64)
{
  std::cout<<"Double precision\n";
}

template<typename FloatType>
void
Rover<FloatType>::about()
{
  std::cout<<"rover version: xx.xx.xx\n";
  print_type(FloatType());
  std::cout<<"Other important information\n";
}
template<typename FloatType>

void
Rover<FloatType>::save_png(const std::string &file_name)
{
  m_internals->save_png(file_name);
}

// Explicit instantiations
template class Rover<vtkm::Float32>; 
template class Rover<vtkm::Float64>; 

}; //namespace rover

