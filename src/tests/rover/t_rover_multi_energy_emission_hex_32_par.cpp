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

#include <mpi.h>

using namespace rover;


TEST(rover_hex, test_call)
{

  MPI_Init(NULL, NULL);

  try {

  vtkmCamera camera;
  std::vector<vtkmDataSet> datasets;
  set_up_lulesh(datasets, camera);
  camera.SetWidth(1024);
  camera.SetHeight(1024);
  std::cout<<"Num Data sets "<<datasets.size()<<"\n";
  const int num_bins = 10;


  add_absorption_field(datasets, "speed", num_bins, vtkm::Float32());
  add_emission_field(datasets, "speed", num_bins, vtkm::Float32());

  CameraGenerator32 generator(camera);
  Rover32 driver32;
  driver32.init(MPI_COMM_WORLD);

  RenderSettings settings;
  settings.m_primary_field   = "absorption";
  settings.m_secondary_field = "emission";
  settings.m_render_mode = rover::energy;
  //settings.m_path_lengths = true;
   
  driver32.set_render_settings(settings);

  for(int i = 0; i < datasets.size(); ++i)
  {
    std::cout<<"Adding data set "<<i<<"\n";
    datasets[i].PrintSummary(std::cout);
    std::cout<<"Done printing "<<i<<"\n";
    driver32.add_data_set(datasets[i]);
  }

  driver32.set_ray_generator(&generator);
  driver32.execute();
  driver32.save_png("multi_energy_hex32_emission_par");

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

