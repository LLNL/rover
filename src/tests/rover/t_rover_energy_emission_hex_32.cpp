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

#include <gtest/gtest.h>
#include "test_utils.hpp"
#include <iostream>
#include <rover.hpp>
#include <rover_exceptions.hpp>
#include <ray_generators/camera_generator.hpp>
#include <utils/vtk_dataset_reader.hpp>

using namespace rover;


TEST(rover_hex, rover_emission_hex)
{

  try {
  vtkmCamera camera;
  vtkmDataSet dataset;
  set_up_lulesh(dataset, camera);
  const int num_bins = 10;
  std::vector<vtkm::cont::DataSet> datasets;
  datasets.push_back(dataset);
  add_absorption_field(datasets, "speed", num_bins, vtkm::Float32());
  add_emission_field(datasets, "speed", num_bins, vtkm::Float32());
  
  CameraGenerator generator(camera, 512, 512);
  Rover driver;
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_primary_field   = "absorption";
  settings.m_secondary_field = "emission";
  settings.m_render_mode = rover::energy;

  driver.add_data_set(datasets[0]);
  driver.set_render_settings(settings);
  driver.set_ray_generator(&generator);
  driver.execute();
  driver.save_png("hex32_emission");

  driver.finalize();  
  }
  catch ( const RoverException &e )
  {
    std::cout<<e.what();
  }
  catch (vtkm::cont::Error error)
  {
    std::cout<<"VTKM exception "<<error.GetMessage()<<"\n";
  }
}

