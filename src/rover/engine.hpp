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
  virtual void init_rays(Ray32 &rays) = 0;
  virtual void init_rays(Ray64 &rays) = 0;
  virtual void set_primary_range(const vtkmRange &range) = 0;
  virtual void set_composite_background(bool on) = 0;
  virtual vtkmRange get_primary_range() = 0;

  virtual void set_primary_field(const std::string &primary_field) = 0;

  void set_secondary_field(const std::string &secondary_field)
  {
    m_secondary_field = secondary_field;
  }

  void set_color_table(const vtkmColorTable &color_map, int samples = 1024)
  {
    color_map.Sample(samples, m_color_map);
  }

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
  std::string m_primary_field;
  std::string m_secondary_field;
};

}; // namespace rover
#endif
