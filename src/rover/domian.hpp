#ifndef rover_domain_h
#define rover_domain_h

namespace rover {

class Domain
{
public:
  Domain();
  ~Domain();
  vtkmDataSet get_data_set();
  void trace(Ray32 &rays);
  void trace(Ray64 &rays);
  void set_data_set(vtkmDataSet &dataset);
  void set_engine(const RenderSettings &setttings);
protected:
  Engine *m_engine;
  vtkmDataSet m_data_set;
}; // class domain
} // namespace rover
#endif
