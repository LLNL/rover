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
VolumeEngine::set_dataset(vtkm::cont::DataSet &dataset)
{
  if(m_tracer) delete m_tracer;
  m_tracer = new vtkm::rendering::ConnectivityProxy(dataset);
}


}; //namespace rover
