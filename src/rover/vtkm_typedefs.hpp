#ifndef rover_vtkm_typedefs_h
#define rover_vtkm_typedefs_h
#include <vtkm/cont/DataSet.h>
#include <vtkm/rendering/ColorTable.h>
#include <vtkm/rendering/raytracing/Ray.h>

namespace rover {
namespace vtkmRayTracing = vtkm::rendering::raytracing;
typedef vtkm::cont::DataSet vtkmDataSet;
typedef vtkm::rendering::raytracing::Ray<vtkm::Float32> Ray32;
typedef vtkm::rendering::raytracing::Ray<vtkm::Float64> Ray64;
typedef vtkm::rendering::ColorTable vtkmColorTable;
};
#endif
