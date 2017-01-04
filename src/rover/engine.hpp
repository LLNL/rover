#ifndef rover_engine_h
#define rover_engine_h
#include <rover_types.hpp>
#include <vtkm_typedefs.hpp>
namespace rover {

class Engine
{
public:
  Engine(){};
  virtual ~Engine(){};
   
  virtual void set_data_set(vtkmDataSet &) = 0;
  virtual void trace(Ray32 &rays) = 0;
  virtual void trace(Ray64 &rays) = 0;
};

}; // namespace rover
#endif
