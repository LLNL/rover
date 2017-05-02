#include <energy_engine.hpp>
#include <rover_exceptions.hpp>
#include <utils/rover_logging.hpp>
namespace rover {

struct ArraySizeFunctor
{
  vtkm::Id  *m_size;
  ArraySizeFunctor(vtkm::Int32 *size)
   : m_size(size)
  {}

  template<typename T, typename Storage>
  void operator()(const vtkm::cont::ArrayHandle<T, Storage> &array) const
  {
    *m_size = array.GetPortalConstControl().GetNumberOfValues();
  } //operator
};

EnergyEngine::EnergyEngine()
{
  m_tracer = NULL;
}

EnergyEngine::~EnergyEngine()
{
  if(m_tracer) delete m_tracer;
}

void
EnergyEngine::set_data_set(vtkm::cont::DataSet &dataset)
{
  if(m_tracer) delete m_tracer;
  ROVER_INFO("Energy Engine settting data set");
  m_tracer = new vtkm::rendering::ConnectivityProxy(dataset);
  m_data_set = dataset;
  m_tracer->SetScalarField(this->m_primary_field);
}


void 
EnergyEngine::set_primary_field(const std::string &primary_field)
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
EnergyEngine::trace(Ray32 &rays)
{
  if(m_tracer == NULL)
  {
    std::cout<<"Engery Engine Error: must set the data set before tracing\n";
  }
  
  if(this->m_primary_field == "")
  {
    throw RoverException("Energy Engine : primary field is not set. Unable to render\n");
  }

  ROVER_INFO("Energy Engine trace32");
#warning "Make method in channel buffer to set number of channels on device"
#warning "Who sets init of rays when rays are passed in from bounce"
  int num_bins = detect_num_bins();

  rays.Buffers.at(0).SetNumChannels(num_bins);
  rays.Buffers.at(0).InitConst(1.);

  m_tracer->SetRenderMode(vtkm::rendering::ConnectivityProxy::ENERGY_MODE);
  m_tracer->SetColorMap(m_color_map);
  m_tracer->Trace(rays);

}

void 
EnergyEngine::init_rays(Ray32 &rays)
{
  
  int num_bins = detect_num_bins();
  rays.Buffers.at(0).SetNumChannels(num_bins);
  rays.Buffers.at(0).InitConst(1.);

}

void 
EnergyEngine::init_rays(Ray64 &rays)
{
  
  int num_bins = detect_num_bins();
  rays.Buffers.at(0).SetNumChannels(num_bins);
  rays.Buffers.at(0).InitConst(1.);

}

void 
EnergyEngine::trace(Ray64 &rays)
{
  if(m_tracer == NULL)
  {
    ROVER_ERROR("energy engine: tracer is NULL data set was never set.");
    std::cout<<"Volume Engine Error: must set the data set before tracing\n";
  }

  if(this->m_primary_field == "")
  {
    throw RoverException("Energy Engine : primary field is not set. Unable to render\n");
  }

  ROVER_INFO("Energy Engine trace64");

  m_tracer->SetRenderMode(vtkm::rendering::ConnectivityProxy::ENERGY_MODE);
  m_tracer->SetColorMap(m_color_map);
  m_tracer->Trace(rays);
}

int 
EnergyEngine::detect_num_bins()
{
  vtkm::Id absorption_size = 0;
  ArraySizeFunctor functor(&absorption_size);
  m_data_set.GetField(this->m_primary_field).GetData().CastAndCall(functor);
  vtkm::Id num_cells = m_data_set.GetCellSet().GetNumberOfCells();

  assert(num_cells > 0);
  assert(absorption_size > 0);

  vtkm::Id modulo = absorption_size % num_cells;
  if(modulo != 0)
  {
    ROVER_ERROR("Absoption does not evenly divided the number of cells. Mod = "<<modulo
                <<"\n        num cells "<<num_cells 
                <<"\n        field size "<<absorption_size);

    throw RoverException("absorption field size invalid (Is not evenly divided by number of cells\n");
  }
  vtkm::Id num_bins = absorption_size / num_cells;
  ROVER_INFO("Dectected "<<num_bins<<" bins ");
  return static_cast<int>(num_bins);
}

vtkmRange
EnergyEngine::get_primary_range()
{
  return m_tracer->GetScalarRange();
}

void 
EnergyEngine::set_composite_background(bool on)
{
  m_tracer->SetCompositeBackground(on);
};

void
EnergyEngine::set_primary_range(const vtkmRange &range)
{
  return m_tracer->SetScalarRange(range);
}

}; //namespace rover
