#ifndef rover_vtkm_typedefs_h
#define rover_vtkm_typedefs_h

#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DeviceAdapterListTag.h>
#include <vtkm/rendering/ColorTable.h>
#include <vtkm/rendering/raytracing/Camera.h>
#include <vtkm/rendering/raytracing/Ray.h>

namespace rover {
namespace vtkmRayTracing = vtkm::rendering::raytracing;
typedef vtkm::cont::DataSet                                   vtkmDataSet;
typedef vtkm::cont::CoordinateSystem                          vtkmCoordinates;
typedef vtkm::rendering::raytracing::Ray<vtkm::Float32>       Ray32;
typedef vtkm::rendering::raytracing::Ray<vtkm::Float64>       Ray64;
typedef vtkm::rendering::ColorTable                           vtkmColorTable;
typedef vtkm::cont::ArrayHandle<vtkm::Vec<vtkm::Float32, 4> > vtkmColorMap;
typedef vtkm::cont::ArrayHandle<vtkm::Vec<vtkm::Float32, 4> > vtkmColorBuffer;
typedef vtkm::rendering::raytracing::Camera                   vtkmCamera;
typedef vtkm::cont::ArrayHandle<vtkm::Id>                     IdHandle;
//
// Utility method for getting raw pointer 
//
template<typename T>
T * 
get_vtkm_ptr(vtkm::cont::ArrayHandle<T> handle)
{
  typedef typename vtkm::cont::ArrayHandle<T>  HandleType;
  typedef typename HandleType::template ExecutionTypes<vtkm::cont::DeviceAdapterTagSerial>::Portal PortalType;
  typedef typename vtkm::cont::ArrayPortalToIterators<PortalType>::IteratorType IteratorType;
  
  IteratorType iter = vtkm::cont::ArrayPortalToIterators<PortalType>(handle.GetPortalControl()).GetBegin();
  return &(*iter);
}

};
#endif
