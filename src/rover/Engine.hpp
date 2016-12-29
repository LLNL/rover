#ifndef rover_engine_h
#define rover_engine_h
namespace rover {

class Engine
{
public:
  Engine();
  ~Engine();
   
  virtual void set_dataset(vtkm::cont::DataSet &) = 0;
};

}; // namespace rover
#endif
