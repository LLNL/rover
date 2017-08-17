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
  template<typename Precision>
  void init_emission(vtkm::rendering::raytracing::Ray<Precision> &rays,
                     const int num_bins);
public:
  EnergyEngine();
  ~EnergyEngine();

  void set_data_set(vtkm::cont::DataSet &);
  void trace(Ray32 &rays);
  void trace(Ray64 &rays);
  void init_rays(Ray32 &rays);
  void init_rays(Ray64 &rays);
  void set_primary_range(const vtkmRange &range) override;
  void set_primary_field(const std::string &primary_field) override;
  void set_secondary_field(const std::string &field);
  void set_composite_background(bool on);
  vtkmRange get_primary_range();
};

}; // namespace rover
#endif
