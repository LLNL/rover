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
  set_up_lulesh(dataset, camera);
  std::vector<vtkm::cont::DataSet> datasets;
  datasets.push_back(dataset);
  const int num_bins = 10;
  add_absorption_field(datasets, "speed", num_bins, vtkm::Float32());

  CameraGenerator generator(camera);
  Rover driver;
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_primary_field = "absorption";
  settings.m_render_mode = rover::energy;
  settings.m_path_lengths = true;

  driver.set_render_settings(settings);
  driver.add_data_set(datasets[0]);
  driver.set_ray_generator(&generator);
  driver.execute();
  driver.save_png("hex32");

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

