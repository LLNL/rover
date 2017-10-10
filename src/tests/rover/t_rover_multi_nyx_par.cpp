/*******************************************************************************
*
*  ROVER HEADER
*
*******************************************************************************/

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

  VisitGenerator32::VisitParams params;

  params.m_focus[0] = 1;
  params.m_focus[1] = 1;
  params.m_focus[2] = 1;

  params.m_view_up[0] = 0;
  params.m_view_up[1] = 1;
  params.m_view_up[2] = 0;

  params.m_normal[0] = 0;
  params.m_normal[1] = 0;
  params.m_normal[2] = 1;

  params.m_image_dims[0] = 500;
  params.m_image_dims[1] = 500;

  params.m_image_zoom = 1;
  params.m_parallel_scale = 1.5;
  params.m_view_angle = 30;
  params.m_near_plane = 4;
  params.m_far_plane = -4;
  params.m_perspective = true; 
  VisitGenerator32 generator(params);

  std::vector<vtkmDataSet> datasets;
  set_up_nyx(datasets);

  std::cout<<"Num Data sets "<<datasets.size()<<"\n";


  Rover32 driver;
  driver.init(MPI_COMM_WORLD);

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

