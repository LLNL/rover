#ifndef test_utils_h
#define test_utils_h

#include "test_config.h"
#include "vtkm_utils.hpp"
#include <vtkm_typedefs.hpp>
#include <utils/vtk_dataset_reader.hpp>

void set_up_zoo(rover::vtkmDataSet &dataset, rover::vtkmCamera &camera)
{
  rover::VTKReader reader;
  std::string data_dir(DATA_DIR);
  std::string file("zoo_slim.vtk");
  std::string file_name = data_dir + file;
  std::cout<<"Reading file "<<file_name<<"\n";
  reader.read_file(file_name);
  dataset = reader.get_data_set();
 
  dataset.PrintSummary(std::cout); 
  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;

  Vec3f position(1.5f, 3.5, -5.5f); 
  Vec3f up(.0f, 1.f, 0.f); 
  Vec3f look_at(1.5f, 1.5f, .5f);
  const int image_width = 500;
  const int image_height = 500;
    
  camera.SetHeight(image_height);
  camera.SetWidth(image_width);
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetUp(up);

}
void set_up_astro(rover::vtkmDataSet &dataset, rover::vtkmCamera &camera)
{
  rover::VTKReader reader;
  std::string data_dir(DATA_DIR);
  std::string file("astro10Uni.vtk");
  std::string file_name = data_dir + file;
  std::cout<<"Reading file "<<file_name<<"\n";
  reader.read_file(file_name);
  dataset = reader.get_data_set();
 
  dataset.PrintSummary(std::cout); 
  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;

  Vec3f position(-10.f, -10.f, -10.f); 
  Vec3f up(.0f, 1.f, 0.f); 
  Vec3f look_at(5.f, 5.f, 5.f);
  const int image_width = 500;
  const int image_height = 500;
    
  camera.SetHeight(image_height);
  camera.SetWidth(image_width);
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetUp(up);

}

void set_up_lulesh(rover::vtkmDataSet &dataset, rover::vtkmCamera &camera)
{
  rover::VTKReader reader;
  std::string data_dir(DATA_DIR);
  std::string file("luleshSpeed.vtk");
  std::string file_name = data_dir + file;
  std::cout<<"Reading file "<<file_name<<"\n";
  reader.read_file(file_name);
  dataset = reader.get_data_set();
 
  dataset.PrintSummary(std::cout); 
  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;

  Vec3f position(-2.5f, -2.5f, -2.5f); 
  Vec3f up(1.f, 0.f, 0.f); 
  Vec3f look_at(.5f, .5f, .5f);
  const int image_width = 500;
  const int image_height = 500;
    
  camera.SetHeight(image_height);
  camera.SetWidth(image_width);
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetUp(up);

}

void set_up_hardy(rover::vtkmDataSet &dataset)
{
  rover::VTKReader reader;
  std::string data_dir(DATA_DIR);
  std::string file("hardy.vtk");
  std::string file_name = data_dir + file;
  std::cout<<"Reading file "<<file_name<<"\n";
  reader.read_file(file_name);
  dataset = reader.get_data_set();
 
  dataset.PrintSummary(std::cout); 
}

void set_up_lulesh(std::vector<rover::vtkmDataSet> &datasets, rover::vtkmCamera &camera)
{
  rover::MultiDomainVTKReader reader;
  std::string data_dir(DATA_DIR);
  data_dir += "multi_domain/";
  std::string file("lulesh_multi_domain.visit");
  reader.read_file(data_dir, file);
  datasets = reader.get_data_sets();
 
  //dataset.PrintSummary(std::cout); 
  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;

  Vec3f position(-2.5f, -2.5f, -2.5f); 
  Vec3f up(1.f, 0.f, 0.f); 
  Vec3f look_at(.5f, .5f, .5f);
  const int image_width = 500;
  const int image_height = 500;
    
  camera.SetHeight(image_height);
  camera.SetWidth(image_width);
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetUp(up);

}

#endif

