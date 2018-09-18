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
#include <ray_generators/visit_generator.hpp>
#include <utils/vtk_dataset_reader.hpp>

#include <mpi.h>

using namespace rover;


TEST(rover_hex, test_call)
{

  MPI_Init(NULL, NULL);

  try {

  VisitGenerator::VisitParams params;

  params.m_focus[0] = 1;
  params.m_focus[1] = 1;
  params.m_focus[2] = 1;

  params.m_view_up[0] = 0;
  params.m_view_up[1] = 1;
  params.m_view_up[2] = 0;

  params.m_normal[0] = 0;
  params.m_normal[1] = 0;
  params.m_normal[2] = 1;

  params.m_image_zoom = 1;
  params.m_parallel_scale = 1.5;
  params.m_view_angle = 30;
  params.m_near_plane = 4;
  params.m_far_plane = -4;
  params.m_perspective = true; 
  VisitGenerator generator(params);

  std::vector<vtkmDataSet> datasets;
  set_up_nyx(datasets);

  std::cout<<"Num Data sets "<<datasets.size()<<"\n";


  Rover driver;
  driver.set_mpi_comm_handle(MPI_Comm_c2f(MPI_COMM_WORLD));

  RenderSettings settings;
  settings.m_primary_field   = "simdata";
  settings.m_secondary_field = "simdata";
  settings.m_render_mode = rover::energy;
  //settings.m_path_lengths = true;
   
  driver.set_render_settings(settings);

  for(int i = 0; i < datasets.size(); ++i)
  {
    driver.add_data_set(datasets[i]);
  }

  driver.set_ray_generator(&generator);
  driver.execute();
  driver.save_png("multi_nyx_par");

  //driver32.finalize();
  MPI_Finalize();

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

