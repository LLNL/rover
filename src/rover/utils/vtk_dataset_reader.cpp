#include <utils/vtk_dataset_reader.hpp>
#include <vtkm/io/reader/VTKDataSetReader.h>

namespace rover {

VTKReader::VTKReader()
{
}
void 
VTKReader::read_file(const std::string &file_name)
{
  vtkm::io::reader::VTKDataSetReader reader(file_name.c_str());
  m_dataset= reader.ReadDataSet();

}
//void add_absorption();
//void add_emmission();
vtkmDataSet
VTKReader::get_dataset()
{
  return m_dataset;
}

} // namespace rover
