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
#include <ray_generators/camera_generator.hpp>
#include <utils/vtk_dataset_reader.hpp>

using namespace rover;


TEST(rover_hex, test_call)
{

  try {
  vtkmCamera camera;
  vtkmDataSet dataset;
  set_up_zoo(dataset, camera);

  const int num_bins = 10;
  add_absorption_field(dataset, "var_point", num_bins, vtkm::Float32());

  CameraGenerator32 generator(camera,
                              dataset.GetCoordinateSystem() );
  Rover32 driver32;
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_render_mode = rover::energy;
  settings.m_primary_field = "absorption";
  
  driver32.set_render_settings(settings);
  driver32.add_data_set(dataset);
  driver32.set_ray_generator(&generator);
  driver32.execute();
  driver32.save_png("energy_zoo_32");

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

