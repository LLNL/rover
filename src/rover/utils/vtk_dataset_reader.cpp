#include <utils/vtk_dataset_reader.hpp>
#include <vtkm/io/reader/VTKDataSetReader.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

namespace rover {

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{   
    std::stringstream ss(s);
      std::string item;
        while (std::getline(ss, item, delim))
            {   
                   elems.push_back(item);
                     }
          return elems;
           }
 
std::vector<std::string> split(const std::string &s, char delim)
{   
    std::vector<std::string> elems;
      split(s, delim, elems);
        return elems;
}

bool contains(const std::string haystack, std::string needle)
{
    std::size_t found = haystack.find(needle);
      return (found != std::string::npos);
}

VTKReader::VTKReader()
{
}

void 
VTKReader::read_file(const std::string &file_name)
{
  vtkm::io::reader::VTKDataSetReader reader(file_name.c_str());
  m_dataset= reader.ReadDataSet();

}

vtkmDataSet
VTKReader::get_data_set()
{
  return m_dataset;
}

MultiDomainVTKReader::MultiDomainVTKReader()
{
}

void 
MultiDomainVTKReader::read_file(const std::string &directory, const std::string &file_name)
{
  //
  //  Get the list of file names from the main file
  //
  std::string full_name = directory + file_name;
  std::ifstream header_file(full_name.c_str());
  std::string line;
  if(header_file.is_open())
  {
     getline(header_file, line);
     std::cout<<"Line: "<<line<<"\n";
     std::vector<std::string> tokens = split(line,' ');
     if(tokens.size() != 2) 
     {
       std::cout<<"Error reading number of files\n";
       return; 
     }
     const int number_of_domains = atoi(tokens.at(1).c_str());
     std::cout<<"Reading "<<number_of_domains<<" files\n";
     for(int i = 0; i < number_of_domains; ++i)
     {
        getline(header_file, line);
        full_name = directory + line;
        std::cout<<"Reading domain "<<full_name<<"\n";
        vtkm::io::reader::VTKDataSetReader reader(full_name.c_str());
        m_datasets.push_back(reader.ReadDataSet());
        //m_datasets[i].PrintSummary(std::cout);
     }
    
  }
  else
  {
    std::cout<<"Failed to open multi domain header file "<<full_name<<"\n";
    return;
  }

  header_file.close();
}

std::vector<vtkmDataSet>
MultiDomainVTKReader::get_data_sets()
{
  return m_datasets;
}

} // namespace rover
