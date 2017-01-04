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
  
  void set_color_map(const vtkmColorMap &color_map)
  {
    m_color_map = color_map;
  }

  vtkmColorMap get_color_map() const
  {
    return m_color_map;
  }
protected:
  vtkmColorMap m_color_map;
};

}; // namespace rover
#endif
