#ifndef rover_volume_engine_h
#define rover_volume_engine_h

#include <engine.hpp>
#include <vtkm/rendering/ConnectivityProxy.h>
namespace rover {

class VolumeEngine : public Engine
{
protected:
  vtkm::rendering::ConnectivityProxy *m_tracer;
public:
  VolumeEngine();
  ~VolumeEngine();

  void set_data_set(vtkm::cont::DataSet &);
  void trace(Ray32 &rays);
  void trace(Ray64 &rays);
  vtkmColorBuffer get_color_buffer();    
};

}; // namespace rover
#endif
