#ifndef rover_volume_engine_h
#define rover_volume_engine_h

#include <Engine.hpp>

namespace rover {

class VolumeEngine
{
protected:
  vtkm::rendering::ConnectivityProxy *m_tracer;
public:
  VolumeEngine();
  ~VolumeEngine();

  void set_dataset(vtkm::cont::DataSet &);
  
};

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

}; // namespace rover
#endif
