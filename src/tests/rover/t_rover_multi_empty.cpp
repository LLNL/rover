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


TEST(rover_hex, test_call)
{

  try {
  vtkmCamera camera;
  std::vector<vtkmDataSet> datasets;
  set_up_lulesh(datasets, camera);
  
  camera.SetWidth(512);
  camera.SetHeight(512);
  vtkm::Vec<vtkm::Float32,3> look(-10., -10., -10);
  camera.SetLookAt(look);
  CameraGenerator generator(camera);
  Rover driver32;
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_primary_field = "speed";
  vtkmColorTable color_table("cool2warm");
  color_table.AddAlphaControlPoint(0.0, .01);
  color_table.AddAlphaControlPoint(0.5, .02);
  color_table.AddAlphaControlPoint(1.0, .01);
  settings.m_color_table = color_table;
   
  driver32.set_render_settings(settings);
  for(int i = 0; i < datasets.size(); ++i)
  {
    driver32.add_data_set(datasets[i]);
  }
  driver32.set_ray_generator(&generator);
  driver32.execute();
  driver32.save_png("multi_empty");

  driver32.finalize();
  }
  catch ( const RoverException &e )
  {
    std::cout<<e.what();
    ASSERT_EQ("rover_exception", "it_happened");
  }
  catch (vtkm::cont::Error error)
  {
    std::cout<<"VTKM exception "<<error.GetMessage()<<"\n";

    ASSERT_EQ("vtkm_exception", "it_happened");
  }

}

