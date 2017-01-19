#ifndef rover_vtk_dataset_reader_h
#define rover_vtk_dataset_reader_h
#include <string>
#include <vtkm_typedefs.hpp>
namespace rover {

class VTKReader 
{
public:
  VTKReader();
  void read_file(const std::string &file_name);
  //void add_absorption();
  //void add_emmission();
  vtkmDataSet get_data_set();
protected:
  vtkmDataSet m_dataset;
};

} // namespace rover

#endif
