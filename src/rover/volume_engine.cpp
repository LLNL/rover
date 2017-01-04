#include <volume_engine.hpp>

namespace rover {

VolumeEngine::VolumeEngine()
{
  m_tracer = NULL;
}

VolumeEngine::~VolumeEngine()
{
  if(m_tracer) delete m_tracer;
}

void
VolumeEngine::set_data_set(vtkm::cont::DataSet &dataset)
{
  if(m_tracer) delete m_tracer;
  m_tracer = new vtkm::rendering::ConnectivityProxy(dataset);
}


void 
VolumeEngine::trace(Ray32 &rays)
{
  if(m_tracer == NULL)
  {
    std::cout<<"Volume Engine Error: must set the data set before tracing\n";
  }
}

void 
VolumeEngine::trace(Ray64 &rays)
{
  if(m_tracer == NULL)
  {
    std::cout<<"Volume Engine Error: must set the data set before tracing\n";
  }

}
}; //namespace rover
