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
#ifndef vtkm_utils_h
#define vtkm_utils_h

#include <utils/rover_logging.hpp>
#include <vtkm/filter/CellAverage.h>
#include "material_database.hpp"
#include <assert.h>
#ifdef PARALLEL
#include <mpi.h>
#endif
template<typename OutType>
void get_cell_assoc_field(vtkm::cont::DataSet &dataset,
                          std::string field_name,
                          vtkm::cont::Field &output_field,
                          OutType t)
{

  bool is_assoc_points = dataset.GetField(field_name).GetAssociation() == vtkm::cont::Field::Association::POINTS;
  std::string output_name;
  if(!is_assoc_points)
  {
    output_name = field_name;    
    output_field = dataset.GetField(field_name);
  }
  else
  {
    ROVER_INFO("Re-centering point associated field "<<field_name);
    vtkm::cont::DataSet result; 
    vtkm::filter::CellAverage cell_average;
    output_name = field_name + "_cell"; 
    cell_average.SetOutputFieldName(output_name);
    cell_average.SetActiveField(field_name);
    result = cell_average.Execute( dataset );
    vtkm::cont::ArrayHandle<OutType> out_array;
    out_array.Allocate(dataset.GetCellSet().GetNumberOfCells());
    output_field = result.GetField(output_name);
    //output_field = vtkm::cont::Field(  output_name,
    //                                   vtkm::cont::Field::ASSOC_CELL_SET,
    //                                   dataset.GetField(field_name).GetAssocCellSet(),
    //                                   out_array);
  }
  std::cout<<"Result field name : "<<output_name<<"\n";
}

template<typename OutType>
struct IdToMaterialFunctor
{
  vtkm::Id m_size; // input array size
  vtkm::Id m_bins;
  vtkm::Id m_num_mats;
  vtkm::cont::ArrayHandle<OutType> *m_output_array;
  vtkm::Range m_range;
  vtkm::cont::ArrayHandle<OutType> m_mat_lookup;

  IdToMaterialFunctor(vtkm::Int32 bins,
                         vtkm::Int32 num_mats,
                         vtkm::cont::ArrayHandle<OutType> *output_array,
                         vtkm::Range range,
                         vtkm::cont::ArrayHandle<OutType> mat_lookup)
    : m_bins(bins),
      m_num_mats(num_mats),
      m_output_array(output_array),
      m_range(range),
      m_mat_lookup(mat_lookup)
    {}

  template<typename T, typename Storage>
  void operator()(const vtkm::cont::ArrayHandle<T, Storage> &array) const
  {
    vtkm::Id field_size = array.GetPortalConstControl().GetNumberOfValues();
    m_output_array->Allocate( m_bins * field_size );
    ROVER_INFO("Creating absorption field of size "<<field_size<<" num bins "<<m_bins);
    vtkm::Float32 inv_diff = m_range.Min;
    if(m_range.Max - m_range.Min != 0.f)
      inv_diff = 1.f / (m_range.Max - m_range.Min);
    
    for(int i  = 0; i < field_size; ++i)
    { 
      T field_value = array.GetPortalConstControl().Get(i);
      vtkm::Float32 normalized_value = (field_value - m_range.Min) * inv_diff;
      vtkm::Id mat_index = static_cast<vtkm::Float32>(field_value);

      mat_index = vtkm::Min(m_num_mats - 1, vtkm::Max(vtkm::Id(0), mat_index)); 

      vtkm::Id mat_starting_index = mat_index * m_bins;
      vtkm::Id starting_index = i * m_bins;
      for(int j = 0; j < m_bins; ++j)
      {
        OutType val  = m_mat_lookup.GetPortalConstControl().Get(mat_starting_index + j);
        m_output_array->GetPortalControl().Set(starting_index + j, val);
      } // for num_bins
    }  // for field_size
  } //operator


};
template<typename OutType>
struct FieldToMaterialFunctor
{
  vtkm::Id m_size; // input array size
  vtkm::Id m_bins;
  vtkm::Id m_num_mats;
  vtkm::Range m_range;
  vtkm::cont::ArrayHandle<OutType> m_mat_lookup;
  vtkm::cont::ArrayHandle<OutType> *m_output_array;
  FieldToMaterialFunctor(vtkm::Int32 bins,
                         vtkm::Int32 num_mats,
                         vtkm::cont::ArrayHandle<OutType> *output_array,
                         vtkm::Range range,
                         vtkm::cont::ArrayHandle<OutType> mat_lookup)
    : m_bins(bins),
      m_num_mats(num_mats),
      m_output_array(output_array),
      m_range(range),
      m_mat_lookup(mat_lookup)
    {}

  template<typename T, typename Storage>
  void operator()(const vtkm::cont::ArrayHandle<T, Storage> &array) const
  {
    vtkm::Id field_size = array.GetPortalConstControl().GetNumberOfValues();
    m_output_array->Allocate( m_bins * field_size );
    ROVER_INFO("Creating absorption field of size "<<field_size<<" num bins "<<m_bins);
    vtkm::Float64 inv_diff = m_range.Min;
    if(m_range.Max - m_range.Min != 0.f)
      inv_diff = 1.f / (m_range.Max - m_range.Min);

    for(int i  = 0; i < field_size; ++i)
    { 
      T field_value = array.GetPortalConstControl().Get(i);
      vtkm::Float32 normalized_value = (field_value - m_range.Min) * inv_diff;
      vtkm::Id mat_index = static_cast<vtkm::Float32>(m_num_mats - 1) * normalized_value;

      mat_index = vtkm::Min(m_num_mats - 1, vtkm::Max(vtkm::Id(0), mat_index)); 

      vtkm::Id mat_starting_index = mat_index * m_bins;
      vtkm::Id starting_index = i * m_bins;
      for(int j = 0; j < m_bins; ++j)
      {
        OutType val  = m_mat_lookup.GetPortalConstControl().Get(mat_starting_index + j);
        m_output_array->GetPortalControl().Set(starting_index + j, val);
      } // for num_bins
    }  // for field_size
  } //operator


};

vtkm::Range
get_global_range(std::vector<vtkm::cont::Field> fields)
{
  vtkm::Range range;
  for(int i = 0; i < (int)fields.size(); ++i)
  {
    vtkm::cont::ArrayHandle<vtkm::Range> range_array = fields[i].GetRange();
    vtkm::Range scalar_range = range_array.GetPortalControl().Get(0);
    range.Include(scalar_range);
  }
   
#ifdef PARALLEL
  
    vtkm::Float64 local_min = range.Min;
    vtkm::Float64 local_max = range.Max;
    
    vtkm::Float64 global_min = 0;
    vtkm::Float64 global_max = 0;

    MPI_Allreduce((void *)(&local_min),
                  (void *)(&global_min), 
                  1,
                  MPI_DOUBLE,
                  MPI_MIN,
                  MPI_COMM_WORLD);

    MPI_Allreduce((void *)(&local_max),
                  (void *)(&global_max),
                  1,
                  MPI_DOUBLE,
                  MPI_MAX,
                  MPI_COMM_WORLD);
    range.Min = global_min;
    range.Max = global_max;
#endif

  return range;
}

template<typename FieldType>
void
add_absorption_clock(std::vector<vtkm::cont::DataSet> &datasets, 
                     std::string mapping_field_name, 
                     const int num_bins,
                     FieldType field_type)
{
  //
  //  Make sure the field is cell associated
  //
  std::vector<vtkm::cont::Field> fields; 

  for(int i = 0; i < datasets.size(); ++i)
  {
    vtkm::cont::Field cell_field;
    get_cell_assoc_field(datasets[i], mapping_field_name, cell_field, field_type);
    fields.push_back(cell_field);
  }

  vtkm::Range scalar_range = get_global_range(fields);
  //
  // create the lookup materials table
  //
  rover::MaterialDatabase db_reader;   
  std::vector<std::string> mat_names;
  mat_names.push_back("C");  // glass
  mat_names.push_back("U"); // clock hands
  mat_names.push_back("C"); // back cover
  mat_names.push_back("C"); // bells
  mat_names.push_back("C"); // body 
  mat_names.push_back("C"); // circle
  mat_names.push_back("C"); // dial
  mat_names.push_back("C"); // feet 
  mat_names.push_back("C"); // mallet 
  mat_names.push_back("C"); // needle holder 
  mat_names.push_back("C"); // nuts
  mat_names.push_back("C"); // rods
  mat_names.push_back("C"); // tonfs
  vtkm::cont::ArrayHandle<FieldType> mat_lookup;
  int num_elements;
  db_reader.get_elements(mat_names, num_bins, mat_lookup, num_elements);

  for(int i = 0; i < datasets.size(); ++i)
  {
    vtkm::cont::ArrayHandle<FieldType> output_array;
    IdToMaterialFunctor<FieldType> matFunctor(num_bins,
                                              num_elements,
                                              &output_array,
                                              scalar_range,
                                              mat_lookup);
    //
    // reset the type list to floats and doubles only
    //
    try
    {
        fields[i].GetData().ResetTypeList( vtkm::TypeListTagFieldScalar() ).CastAndCall(matFunctor);
    }
    catch (vtkm::cont::Error error)
    {
      std::cout<<"Failed to add absorption field. mapping field not a floating point value. "<<error.GetMessage()<<"\n";
      return;
    }

    datasets[i].AddField( vtkm::cont::Field(  "absorption",
                          vtkm::cont::Field::Association::CELL_SET,
                          datasets[i].GetField(mapping_field_name).GetAssocCellSet(),
                          output_array));
  }
}
template<typename FieldType>
void
add_absorption_field(std::vector<vtkm::cont::DataSet> &datasets, 
                     std::string mapping_field_name, 
                     const int num_bins,
                     FieldType field_type)
{
  //
  //  Make sure the field is cell associated
  //
  std::vector<vtkm::cont::Field> fields; 

  for(int i = 0; i < datasets.size(); ++i)
  {
    vtkm::cont::Field cell_field;
    get_cell_assoc_field(datasets[i], mapping_field_name, cell_field, field_type);
    fields.push_back(cell_field);
  }

  vtkm::Range scalar_range = get_global_range(fields);
  //
  // create the lookup materials table
  //
  rover::MaterialDatabase db_reader;   
  std::vector<std::string> mat_names;
  mat_names.push_back("O");
  mat_names.push_back("N");
  mat_names.push_back("C");
  mat_names.push_back("Ti");
  vtkm::cont::ArrayHandle<FieldType> mat_lookup;
  int num_elements;
  db_reader.get_elements(mat_names, num_bins, mat_lookup, num_elements);

  for(int i = 0; i < datasets.size(); ++i)
  {
    vtkm::cont::ArrayHandle<FieldType> output_array;
    FieldToMaterialFunctor<FieldType> matFunctor(num_bins,
                                                 num_elements,
                                                 &output_array,
                                                 scalar_range,
                                                 mat_lookup);
    //
    // reset the type list to floats and doubles only
    //
    try
    {
        fields[i].GetData().ResetTypeList( vtkm::TypeListTagFieldScalar() ).CastAndCall(matFunctor);
    }
    catch (vtkm::cont::Error error)
    {
      std::cout<<"Failed to add absorption field. mapping field not a floating point value. "<<error.GetMessage()<<"\n";
      return;
    }

    datasets[i].AddField( vtkm::cont::Field(  "absorption",
                          vtkm::cont::Field::Association::CELL_SET,
                          datasets[i].GetField(mapping_field_name).GetAssocCellSet(),
                          output_array));
  }
}
template<typename FieldType>
void
add_emission_field(std::vector<vtkm::cont::DataSet> &datasets, 
                   std::string mapping_field_name, 
                   const int num_bins,
                   FieldType field_type)
{
  //
  //  Make sure the field is cell associated
  //
  std::vector<vtkm::cont::Field> fields; 

  for(int i = 0; i < datasets.size(); ++i)
  {
    vtkm::cont::Field cell_field;
    get_cell_assoc_field(datasets[i], mapping_field_name, cell_field, field_type);
    fields.push_back(cell_field);
  }

  vtkm::Range scalar_range = get_global_range(fields);
  //
  // create the lookup materials table
  //
  rover::MaterialDatabase db_reader;   
  std::vector<std::string> mat_names;

  mat_names.push_back("Na");
  mat_names.push_back("N");
  mat_names.push_back("Pu");
  mat_names.push_back("U");

  vtkm::cont::ArrayHandle<FieldType> mat_lookup;
  int num_elements;
  db_reader.get_elements(mat_names, num_bins, mat_lookup, num_elements);

  for(int i = 0; i < datasets.size(); ++i)
  {
    vtkm::cont::ArrayHandle<FieldType> output_array;
    FieldToMaterialFunctor<FieldType> matFunctor(num_bins,
                                                 num_elements,
                                                 &output_array,
                                                 scalar_range,
                                                 mat_lookup);
    //
    // reset the type list to floats and doubles only
    //
    try
    {
        fields[i].GetData().ResetTypeList( vtkm::TypeListTagFieldScalar() ).CastAndCall(matFunctor);
    }
    catch (vtkm::cont::Error error)
    {
      std::cout<<"Failed to add absorption field. mapping field not a floating point value. "<<error.GetMessage()<<"\n";
      return;
    }

    
      datasets[i].AddField( vtkm::cont::Field(  "emission",
                            vtkm::cont::Field::Association::CELL_SET,
                            datasets[i].GetField(mapping_field_name).GetAssocCellSet(),
                            output_array));
  }
}

#endif
