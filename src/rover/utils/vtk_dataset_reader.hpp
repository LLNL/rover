#ifndef rover_vtk_dataset_reader_h
#define rover_vtk_dataset_reader_h
#include <string>
#include <vector>
#include <vtkm_typedefs.hpp>
namespace rover {

class VTKReader 
{
public:
  VTKReader();
  void read_file(const std::string &file_name);
  vtkmDataSet get_data_set();
protected:
  vtkmDataSet m_dataset;
};

class MultiDomainVTKReader 
{
public:
  MultiDomainVTKReader();
  void read_file(const std::string &directory, const std::string &file_name);
  std::vector<vtkmDataSet> get_data_sets();
protected:
  std::vector<vtkmDataSet> m_datasets;
};
} // namespace rover

#endif
