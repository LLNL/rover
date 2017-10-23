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

using namespace rover;


TEST(rover_hex, test_call)
{

  try {
  vtkmDataSet dataset;
  set_up_hardy(dataset);
  VisitGenerator::VisitParams params;

  params.m_focus[0] = 0;
  params.m_focus[1] = 0;
  params.m_focus[2] = 0;

  params.m_view_up[0] = 0;
  params.m_view_up[1] = 1;
  params.m_view_up[2] = 0;

  params.m_normal[0] = 0;
  params.m_normal[1] = 0;
  params.m_normal[2] = 1;

  params.m_image_dims[0] = 500;
  params.m_image_dims[1] = 500;

  params.m_image_zoom = 1;
  params.m_parallel_scale = 10;
  params.m_view_angle = 30;
  params.m_near_plane = -20;
  params.m_far_plane = 20;
  params.m_perspective = false; 

  VisitGenerator generator(params);
  Rover driver;
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_primary_field = "z";
  settings.m_secondary_field = "z";
  settings.m_render_mode = rover::energy;
  //settings.m_render_mode = rover::volume;
  //settings.m_path_lengths = true;

  driver.set_render_settings(settings);
  driver.add_data_set(dataset);
  driver.set_ray_generator(&generator);
  driver.execute();
  driver.save_png("hardy");

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

