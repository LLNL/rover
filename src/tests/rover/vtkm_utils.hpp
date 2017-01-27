#ifndef vtkm_utils_h
#define vtkm_utils_h

#include <utils/rover_logging.hpp>
#include <vtkm/filter/CellAverage.h>
#include "material_database.hpp"
#include <assert.h>

template<typename OutType>
void get_cell_assoc_field(vtkm::cont::DataSet &dataset,
                          std::string field_name,
                          vtkm::cont::Field &output_field,
                          OutType t)
{

  bool is_assoc_points = dataset.GetField(field_name).GetAssociation() == vtkm::cont::Field::ASSOC_POINTS;
  std::string output_name;
  if(!is_assoc_points)
  {
    output_name = field_name;    
    output_field = dataset.GetField(field_name);
  }
  else
  {
    ROVER_INFO("Re-centering point associated field "<<field_name);
    vtkm::filter::ResultField result; 
    vtkm::filter::CellAverage cell_average;
    cell_average.SetOutputFieldName("test_ave");
    result = cell_average.Execute( dataset, dataset.GetField(field_name));
    vtkm::cont::ArrayHandle<OutType> out_array;
    out_array.Allocate(dataset.GetCellSet().GetNumberOfCells());
    bool valid = result.FieldAs(out_array);
    if(valid) std::cout<<"VALID\n";
    if(!valid) std::cout<<"InVALID\n";
    output_name = field_name + "_cell"; 
    output_field = vtkm::cont::Field(  output_name,
                                       vtkm::cont::Field::ASSOC_CELL_SET,
                                       dataset.GetField(field_name).GetAssocCellSet(),
                                       out_array);
  }
  std::cout<<"Result field name : "<<output_name<<"\n";
}

template<typename OutType>
struct FieldToMaterialFunctor
{
  vtkm::Id m_size; // input array size
  vtkm::Int32 m_bins;
  vtkm::Int32 m_num_mats;
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
    vtkm::Float32 inv_diff = m_range.Min;
    if(m_range.Max - m_range.Min != 0.f)
      inv_diff = 1.f / (m_range.Max - m_range.Min);

    for(int i  = 0; i < field_size; ++i)
    { 
      T field_value = array.GetPortalConstControl().Get(i);
      vtkm::Float32 normalized_value = (field_value - m_range.Min) * inv_diff;
      vtkm::Id mat_index = static_cast<vtkm::Float32>(m_num_mats - 1) * normalized_value;

      mat_index = vtkm::Min(m_num_mats - 1, vtkm::Max(0, mat_index)); 

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

template<typename FieldType>
void
add_absorption_field(vtkm::cont::DataSet &dataset, 
                     std::string mapping_field_name, 
                     const int num_bins,
                     FieldType field_type)
{
  //
  //  Make sure the field is cell associated
  //
  vtkm::cont::Field cell_field;
  get_cell_assoc_field(dataset, mapping_field_name, cell_field, field_type);
  vtkm::cont::ArrayHandle<vtkm::Range> range_array = cell_field.GetRange(vtkm::cont::DeviceAdapterTagSerial());
  assert(range_array.GetPortalControl().GetNumberOfValues() == 1);
  vtkm::Range scalar_range = range_array.GetPortalControl().Get(0);
  
  //
  // create the lookup materials table
  //
  rover::MaterialDatabase db_reader;   
  std::vector<std::string> mat_names;
  mat_names.push_back("O");
  mat_names.push_back("N");
  mat_names.push_back("Ti");
  mat_names.push_back("C");
  vtkm::cont::ArrayHandle<FieldType> mat_lookup;
  int num_elements;
  db_reader.get_elements(mat_names, num_bins, mat_lookup, num_elements);
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
      cell_field.GetData().ResetTypeList( vtkm::TypeListTagFieldScalar() ).CastAndCall(matFunctor);
  }
  catch (vtkm::cont::Error error)
  {
    std::cout<<"Failed to add absorption field. mapping field not a floating point value. "<<error.GetMessage()<<"\n";
    return;
  }

  
    dataset.AddField( vtkm::cont::Field(  "absorption",
                                          vtkm::cont::Field::ASSOC_CELL_SET,
                                          dataset.GetField(mapping_field_name).GetAssocCellSet(),
                                          output_array));
}

#endif
