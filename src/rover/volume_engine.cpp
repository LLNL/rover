#include <volume_engine.hpp>
#include <rover_exceptions.hpp>
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
  
  if(this->m_primary_field == "")
  {
    throw RoverException("Primary field is not set. Unable to render\n");
  }

  m_tracer->SetScalarField(this->m_primary_field);
  m_tracer->SetColorMap(m_color_map);
  m_tracer->Trace(rays);

}

void 
VolumeEngine::trace(Ray64 &rays)
{
  if(m_tracer == NULL)
  {
    std::cout<<"Volume Engine Error: must set the data set before tracing\n";
  }

  m_tracer->SetScalarField(this->m_primary_field);
  m_tracer->SetColorMap(m_color_map);
  m_tracer->Trace(rays);
}

}; //namespace rover
