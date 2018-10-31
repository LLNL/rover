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
#include <vtkm_utils.hpp>
#include "open_simplex_noise.h"

#include <rover.hpp>
#include <ray_generators/camera_generator.hpp>

#include <assert.h>
#include <iostream>
#include <sstream>
#ifdef PARALLEL
#include <mpi.h>
#endif

using namespace rover;

struct Options
{
  int    m_dims[3];
  double m_spacing[3];
  int    m_time_steps;
  double m_time_delta;
  int    m_bins;
  bool   m_double_precision;
  Options()
    : m_dims{32,32,32},
      m_time_steps(10),
      m_time_delta(0.5),
      m_bins(-1),
      m_double_precision(false)
  {
    SetSpacing();
  }
  void SetSpacing()
  {
    m_spacing[0] = 10. / double(m_dims[0]);
    m_spacing[1] = 10. / double(m_dims[1]);
    m_spacing[2] = 10. / double(m_dims[2]);
  }
  void Parse(int argc, char** argv)
  {
    for(int i = 1; i < argc; ++i)
    {
      if(contains(argv[i], "--dims="))
      {
        std::string s_dims;
        s_dims = GetArg(argv[i]); 
        std::vector<std::string> dims;
        dims = split(s_dims, ',');

        if(dims.size() != 3)
        {
          Usage(argv[i]);
        }

        m_dims[0] = stoi(dims[0]);
        m_dims[1] = stoi(dims[1]);
        m_dims[2] = stoi(dims[2]);
        SetSpacing(); 
      }
      else if(contains(argv[i], "--time_steps="))
      {

        std::string time_steps;
        time_steps = GetArg(argv[i]); 
        m_time_steps = stoi(time_steps); 
      }
      else if(contains(argv[i], "--time_delta="))
      {

        std::string time_delta;
        time_delta= GetArg(argv[i]); 
        m_time_delta = stof(time_delta); 
      }
      else if(contains(argv[i], "--energy="))
      {

        std::string bins;
        bins = GetArg(argv[i]); 
        m_bins = stoi(bins); 
      }
      else if(contains(argv[i], "--double"))
      {
        m_double_precision = true;
      }
      else
      {
        Usage(argv[i]);
      }
    }
  }

  std::string GetArg(const char *arg)
  {
    std::vector<std::string> parse;
    std::string s_arg(arg);
    std::string res;

    parse = split(s_arg, '=');

    if(parse.size() != 2)
    {
      Usage(arg);
    }
    else
    {
      res = parse[1];
    } 
    return res;
  }
  void Print() const
  {
    std::cout<<"======== Noise Options =========\n";
    std::cout<<"dims       : ("<<m_dims[0]<<", "<<m_dims[1]<<", "<<m_dims[2]<<")\n"; 
    std::cout<<"spacing    : ("<<m_spacing[0]<<", "<<m_spacing[1]<<", "<<m_spacing[2]<<")\n"; 
    std::cout<<"time steps : "<<m_time_steps<<"\n"; 
    std::cout<<"time delta : "<<m_time_delta<<"\n"; 
    std::cout<<"================================\n";
  }

  void Usage(std::string bad_arg)
  {
    std::cerr<<"Invalid argument \""<<bad_arg<<"\"\n";
    std::cout<<"Noise usage: "
             <<"       --dims       : global data set dimensions (ex: --dims=32,32,32)\n"
             <<"       --time_steps : number of time steps  (ex: --time_steps=10)\n"
             <<"       --time_delta : amount of time to advance per time step  (ex: --time_delta=0.5)\n"
             <<"       --double : use double precision. Default is single precision\n"
             <<"       --energy : generate engery bins based on scalar fieldp  (ex: --energy=10 [10 energy bins])\n";
    exit(0);
  }

	std::vector<std::string> &split(const std::string &s, 
                                  char delim, 
                                  std::vector<std::string> &elems)
	{   
		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, delim))
		{   
			 elems.push_back(item);
		}
		return elems;
	 }
	 
	std::vector<std::string> split(const std::string &s, char delim)
	{   
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	} 

	bool contains(const std::string haystack, std::string needle)
	{
		std::size_t found = haystack.find(needle);
		return (found != std::string::npos);
	}
};


struct SpatialDivision
{
  int m_mins[3];
  int m_maxs[3];

  SpatialDivision()
    : m_mins{0,0,0},
      m_maxs{1,1,1}
  {

  }

  bool CanSplit(int dim)
  {
    return m_maxs[dim] - m_mins[dim] + 1> 1;
  }

  SpatialDivision Split(int dim)
  {
    SpatialDivision r_split;
    r_split = *this;
    assert(CanSplit(dim));
    int size = m_maxs[dim] - m_mins[dim] + 1;
    int left_offset = size / 2;   
  
    //shrink the left side
    m_maxs[dim] = m_mins[dim] + left_offset - 1;
    //shrink the right side
    r_split.m_mins[dim] = m_maxs[dim] + 1;
    return r_split;    
  }
};

template<typename Precision>
struct DataSet
{
  const int     m_cell_dims[3];
  const int     m_point_dims[3];
  const int     m_cell_size;
  const int     m_point_size;
  Precision    *m_nodal_scalars;
  Precision    *m_nodal2_scalars;
  Precision    *m_zonal_scalars;
  double        m_spacing[3];
  double        m_origin[3];
  Precision     m_time_step;

  struct osn_context *ctx_zonal;
  struct osn_context *ctx_nodal;
  struct osn_context *ctx_nodal2;
  DataSet(const Options &options, const SpatialDivision &div)
    : m_cell_dims{div.m_maxs[0] - div.m_mins[0] + 1, 
                  div.m_maxs[1] - div.m_mins[1] + 1,
                  div.m_maxs[2] - div.m_mins[2] + 1},
      m_point_dims{m_cell_dims[0] + 1, 
                   m_cell_dims[1] + 1, 
                   m_cell_dims[2] + 1},
      m_cell_size(m_cell_dims[0] * m_cell_dims[1] * m_cell_dims[2]),
      m_point_size(m_point_dims[0] * m_point_dims[1] * m_point_dims[2]),
      m_spacing{options.m_spacing[0],
                options.m_spacing[1],
                options.m_spacing[2]},
      m_origin{0. + Precision(div.m_mins[0]) * m_spacing[0],
               0. + Precision(div.m_mins[1]) * m_spacing[1],
               0. + Precision(div.m_mins[2]) * m_spacing[2]}

  {
    open_simplex_noise(77374, &ctx_nodal);
    open_simplex_noise(59142, &ctx_zonal);
    open_simplex_noise(82031, &ctx_nodal2);

    m_nodal_scalars  = new Precision[m_point_size]; 
    m_nodal2_scalars = new Precision[m_point_size]; 
    m_zonal_scalars  = new Precision[m_cell_size]; 
    std::cout<<"Point size "<<m_point_size<<"\n";
    std::cout<<"Cell size "<<m_cell_size<<"\n";
  }    

  inline void GetCoord(const int &x, const int &y, const int &z, double *coord)
  {
     coord[0] = m_origin[0] + m_spacing[0] * Precision(x); 
     coord[1] = m_origin[1] + m_spacing[1] * Precision(y); 
     coord[2] = m_origin[2] + m_spacing[2] * Precision(z); 
  }  

  inline void SetPoint(const double &val, const int &x, const int &y, const int &z)
  {
    const int offset = z * m_point_dims[0] * m_point_dims[1] +
                       y * m_point_dims[0] + x;
    assert(offset >= 0);
    assert(offset < m_point_size);
    m_nodal_scalars[offset] = val;
  } 
  inline void SetPoint2(const double &val, const int &x, const int &y, const int &z)
  {
    const int offset = z * m_point_dims[0] * m_point_dims[1] +
                       y * m_point_dims[0] + x;
    assert(offset >= 0);
    assert(offset < m_point_size);
    m_nodal2_scalars[offset] = val;
  } 

  inline void SetCell(const double &val, const int &x, const int &y, const int &z)
  {
    const int offset = z * m_cell_dims[0] * m_cell_dims[1] +
                       y * m_cell_dims[0] + x;
    assert(offset >= 0);
    assert(offset < m_cell_size);
    m_zonal_scalars[offset] = val;
  } 
  void Update(double time)
  {

    for(int z = 0; z < m_point_dims[2]; ++z)
      for(int y = 0; y < m_point_dims[1]; ++y)
# ifdef NOISE_USE_OPENMP
        #pragma omp parallel for
# endif
        for(int x = 0; x < m_point_dims[0]; ++x)
        {
          double coord[4];
          GetCoord(x,y,z,coord);
          coord[3] = time;
          double val =  open_simplex_noise4(ctx_nodal, coord[0], coord[1], coord[2], coord[3]);
          double val2 = open_simplex_noise4(ctx_nodal2, coord[0], coord[1], coord[2], coord[3]);
          double val3 = open_simplex_noise4(ctx_zonal, coord[0], coord[1], coord[2], coord[3]);
          SetPoint(val,x,y,z);
          SetPoint2(val2,x,y,z);
          if(x < m_cell_dims[0] &&
             y < m_cell_dims[1] &&
             z < m_cell_dims[2]) 
          {
            SetCell(val3, x, y, z);
          }

        }
  }

  vtkmDataSet GetVtkmUniformDataSet()
  {
    vtkmDataSet res;
    vtkmVec3f origin(m_origin[0], m_origin[1], m_origin[2]); 
    vtkmVec3f spacing(m_spacing[0], m_spacing[1], m_spacing[2]); 
    vtkm::Id3 dims(m_point_dims[0], m_point_dims[1], m_point_dims[2]);   
    res.AddCoordinateSystem( vtkm::cont::CoordinateSystem("uniform_coords",
                                                          dims,
                                                          origin,
                                                          spacing));

    vtkm::cont::CellSetStructured<3> cell_set("cell_set");
    cell_set.SetPointDimensions(dims);
    res.AddCellSet(cell_set);
    AddFields(res);
    return res;
  }

  void AddFields(vtkmDataSet &data_set)
  {
     vtkm::cont::ArrayHandle<Precision> vtkm_nodal 
       = vtkm::cont::make_ArrayHandle(m_nodal_scalars, m_point_size);

     vtkm::cont::ArrayHandle<Precision> vtkm_nodal2 
       = vtkm::cont::make_ArrayHandle(m_nodal2_scalars, m_point_size);

     vtkm::cont::ArrayHandle<Precision> vtkm_zonal 
       = vtkm::cont::make_ArrayHandle(m_zonal_scalars, m_cell_size);

     data_set.AddField( vtkm::cont::Field("nodal_noise",
                                          vtkm::cont::Field::Association::POINTS,
                                          vtkm_nodal) );

     data_set.AddField( vtkm::cont::Field("nodal_noise2",
                                          vtkm::cont::Field::Association::POINTS,
                                          vtkm_nodal2) );

     data_set.AddField( vtkm::cont::Field("zonal_noise",
                                          vtkm::cont::Field::Association::CELL_SET,
                                          "cell_set",
                                          vtkm_zonal) );
  }

  void Print()
  {
    std::cout<<"Origin "<<"("<<m_origin[0]<<" -  "
                        <<m_origin[0] + m_spacing[0] * m_cell_dims[0]<<"), "
                        <<"("<<m_origin[1]<<" -  "
                        <<m_origin[1] + m_spacing[1] * m_cell_dims[1]<<"), "
                        <<"("<<m_origin[2]<<" -  "
                        <<m_origin[2] + m_spacing[2] * m_cell_dims[2]<<")\n ";
  }

  ~DataSet()
  {
    if(m_nodal_scalars) delete[] m_nodal_scalars; 
    if(m_nodal_scalars) delete[] m_nodal2_scalars; 
    if(m_zonal_scalars) delete[] m_zonal_scalars; 
    open_simplex_noise_free(ctx_nodal);
    open_simplex_noise_free(ctx_zonal);
    open_simplex_noise_free(ctx_nodal2);
  }
private:
DataSet()
: m_cell_dims{1,1,1},
  m_point_dims{2,2,2},
  m_cell_size(1),
  m_point_size(8)
{
  m_nodal_scalars = NULL; 
  m_zonal_scalars = NULL; 
};
};


void Init(SpatialDivision &div, const Options &options)
{
#ifdef PARALLEL

  MPI_Init(NULL,NULL);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank == 0) options.Print(); 
  std::vector<SpatialDivision> divs; 
  divs.push_back(div);
  int assigned = 1;
  int avail = comm_size - 1;
  int current_dim = 0;
  int missed_splits = 0;
  const int num_dims = 3;
  while(avail > 0)
  {
    const int current_size = divs.size();
    int temp_avail = avail;
    for(int i = 0; i < current_size; ++i)
    {
      if(avail == 0) break;
      if(!divs[i].CanSplit(current_dim))
      {
        continue;
      }
      divs.push_back(divs[i].Split(current_dim));
      --avail;
    }      
    if(temp_avail == avail)
    {
      // dims were too small to make any spit
      missed_splits++;
      if(missed_splits == 3)
      {
        // we tried all three dims and could
        // not make a split.
        for(int i = 0; i < avail; ++i)
        {
          SpatialDivision empty;
          empty.m_maxs[0] = 0;
          empty.m_maxs[1] = 0;
          empty.m_maxs[2] = 0;
          divs.push_back(empty);
        }
        if(rank == 0)
        {
          std::cerr<<"** Warning **: data set size is too small to"
                   <<" divide between "<<comm_size<<" ranks. "
                   <<" Adding "<<avail<<" empty data sets\n";
        }

        avail = 0; 
      }
    }
    else
    {
      missed_splits = 0;
    }

    current_dim = (current_dim + 1) % num_dims;
  }

  div = divs.at(rank);
#endif
  options.Print();
}

void Finalize()
{
#ifdef PARALLEL
  MPI_Finalize();
#endif
}

template<typename Precision>
void TemplateDriver(Options &options, Precision)
{
  
  SpatialDivision div;
  //
  // Inclusive range. Ex cell dim = 32
  // then the div is [0,31] 
  //
  div.m_maxs[0] = options.m_dims[0] - 1; 
  div.m_maxs[1] = options.m_dims[1] - 1; 
  div.m_maxs[2] = options.m_dims[2] - 1; 

  Init(div, options);
  DataSet<float> data_set(options, div); 

  vtkmDataSet vtkm_data_set = data_set.GetVtkmUniformDataSet();
  double time = 0;
  
  //
  //  Opem and setup rover 
  //

  vtkmCamera camera;

  vtkmVec3f position(-10.f, -10.f, -10.f); 
  vtkmVec3f up(.0f, 1.f, 0.f); 
  vtkmVec3f look_at(5.f, 5.f, 5.f);
  const int image_width = 500;
  const int image_height = 500;
    
  camera.SetLookAt(look_at);
  camera.SetPosition(position);
  camera.SetViewUp(up);

  CameraGenerator generator(camera, image_height, image_width);
  Rover driver;
  
  RenderSettings settings;
  bool volume_mode = options.m_bins == -1;
  if(volume_mode)
  {
    settings.m_primary_field = "nodal_noise";
    vtkmColorTable color_table("cool2warm");
    color_table.AddPointAlpha(0.0, .01f);
    color_table.AddPointAlpha(0.5, .02f);
    color_table.AddPointAlpha(1.0, .01f);
    settings.m_color_table = color_table;
  }
  else
  {
    settings.m_primary_field = "absorption";
    settings.m_render_mode = rover::energy;
    std::vector<vtkm::cont::DataSet> datasets;
    datasets.push_back(vtkm_data_set);
    add_absorption_field(datasets, "nodal_noise", options.m_bins, Precision());
  }

  driver.set_ray_generator(&generator);
  driver.set_render_settings(settings);
#ifdef PARALLEL
  driver.set_mpi_comm_handle(MPI_Comm_c2f(MPI_COMM_WORLD));
#endif

  driver.add_data_set(vtkm_data_set);
  for(int t = 0; t < options.m_time_steps; ++t)
  {
    data_set.Update(time);
    driver.execute();
    std::stringstream ss;
    ss<<"noise_"<<t;
    driver.save_png(ss.str());
    time += options.m_time_delta;
  } //for each time step
  

  // 
  // cleanup
  //
  driver.finalize();
  Finalize();
};

int main(int argc, char** argv)
{

  Options options;
  options.Parse(argc, argv);

  if(options.m_double_precision)
  {
    TemplateDriver(options, double());
  }
  else
  {
    TemplateDriver(options, float());
  }

}
