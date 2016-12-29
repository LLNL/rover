#include <volume_engine.hpp>
#include <rover.hpp>
#include <vtkm_typedefs.hpp>
#include <iostream>
namespace rover {

class Rover::InternalsType 
{
protected:
  Engine       *m_engine;
  RenderMode    m_render_mode;
  Promise       m_promise; 
  vtkmDataSet   m_dataset;

  void reset_engine()
  {
    if(m_engine) delete m_engine;

    if(m_render_mode == volume)
    {
      m_engine = new VolumeEngine(m_dataset); 
    }
    else if(m_render_mode == engergy)
    {
      std::cout<<"engergy not implemented\n";
    }
    else if(m_render_mode == ray_tracing)
    {
      std::cout<<"ray tracing not implemented\n";
    }
  }
public: 
  InternalsType()
  {
    m_engine = NULL;
    // defualt render mode
    m_render_mode = volume;
    // start with the loosest assumption
    m_promise = non_contiguous_domains;
  }

  void set_dataset(vtkmDataSet &dataset)
  {
    m_dataset = dataset;
  }
};

Rover::Rover()
  : m_internals( new InternalsType )
{

}

Rover::~Rover()
{
  
}

void
Rover::init()
{
  // initialize
}

void
Rover::finalize()
{
  // finalize
}

void
Rover::set_dataset(vtkmDataSet &dataset)
{
  m_internals->set_dataset(dataset); 
}

}; //namespace rover

