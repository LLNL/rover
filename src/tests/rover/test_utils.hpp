//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2018, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-749865
// 
// All rights reserved.
// 
// This file is part of Rover. 
// 
// Please also read rover/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#ifndef test_utils_h
#define test_utils_h

#include "test_config.h"
#include "vtkm_utils.hpp"
#include <vtkm_typedefs.hpp>
#include <utils/vtk_dataset_reader.hpp>

void set_up_clock(rover::vtkmDataSet &dataset, rover::vtkmCamera &camera)
{
  rover::VTKReader reader;
  std::string data_dir(DATA_DIR);
  std::string file("clock.vtk");
  std::string file_name = data_dir + file;
  std::cout<<"Reading file "<<file_name<<"\n";
  reader.read_file(file_name);
  dataset = reader.get_data_set();
 
  dataset.PrintSummary(std::cout); 

  typedef vtkm::Vec<vtkm::Float32,3> Vec3f;
  
  const int image_width = 500;
  const int image_height = 500;
  vtkm::rendering::Camera cam;
  vtkm::rendering::CanvasRayTracer canvas(image_width, image_height);
  cam.ResetToBounds(dataset.GetCoordinateSystem().GetBounds());
  cam.Azimuth(10.f);
  cam.Elevation(10.f);
  
}

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
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetViewUp(up);

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
    
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetViewUp(up);

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

  vtkm::rendering::Camera c;
  c.ResetToBounds(dataset.GetCoordinateSystem().GetBounds());

  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetViewUp(up);

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
    
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetViewUp(up);

}

void set_up_nyx(std::vector<rover::vtkmDataSet> &datasets)
{
  rover::MultiDomainVTKReader reader;
  std::string data_dir(DATA_DIR);
  data_dir += "multi_domain/";
  std::string file("nyx_small.visit");
  reader.read_file(data_dir, file);
  datasets = reader.get_data_sets();
}

#endif

