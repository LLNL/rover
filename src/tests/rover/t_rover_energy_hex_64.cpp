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
  Rover driver64;
  driver64.set_tracer_precision64();
  //
  // Create some basic setting and color table
  //
  RenderSettings settings;
  settings.m_primary_field = "absorption";
  settings.m_render_mode = rover::energy;

  driver64.set_render_settings(settings);
  driver64.add_data_set(dataset);
  driver64.set_ray_generator(&generator);
  driver64.execute();
  driver64.save_png("hex64");
  driver64.finalize();
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

