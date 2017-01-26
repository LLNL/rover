#ifndef rover_energy_engine_h
#define rover_energy_engine_h

#include <engine.hpp>
#include <vtkm/rendering/ConnectivityProxy.h>
namespace rover {

class EnergyEngine : public Engine
{
protected:
  vtkmDataSet m_data_set;
  vtkm::rendering::ConnectivityProxy *m_tracer;
  int detect_num_bins();
public:
  EnergyEngine();
  ~EnergyEngine();

  void set_data_set(vtkm::cont::DataSet &);
  void trace(Ray32 &rays);
  void trace(Ray64 &rays);
};

}; // namespace rover
#endif
