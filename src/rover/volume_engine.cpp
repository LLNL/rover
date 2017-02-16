#include <volume_engine.hpp>
#include <rover_exceptions.hpp>
#include <utils/rover_logging.hpp>
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
  m_tracer->SetScalarField(this->m_primary_field);
}


void 
VolumeEngine::set_primary_field(const std::string &primary_field)
{
  m_primary_field = primary_field;
  if(m_tracer == NULL)
  {
    return;
  }
  else
  {
    m_tracer->SetScalarField(this->m_primary_field);
  }
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
  ROVER_INFO("tracing  rays");
  rays.Buffers.at(0).InitConst(0.);
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

  if(this->m_primary_field == "")
  {
    throw RoverException("Primary field is not set. Unable to render\n");
  }

  ROVER_INFO("tracing  rays");
  rays.Buffers.at(0).InitConst(0.);
  m_tracer->SetColorMap(m_color_map);
  m_tracer->Trace(rays);
}

vtkmRange
VolumeEngine::get_primary_range()
{
  return m_tracer->GetScalarRange();
}

void
VolumeEngine::set_primary_range(const vtkmRange &range)
{
  return m_tracer->SetScalarRange(range);
}

}; //namespace rover
