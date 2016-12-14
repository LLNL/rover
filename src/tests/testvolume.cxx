#ifndef VTKM_DEVICE_ADAPTER
#define VTKM_DEVICE_ADAPTER VTKM_DEVICE_ADAPTER_SERIAL
#endif

#include <sstream>
#include <string>
#include <vtkm/cont/CellSetExplicit.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/rendering/MapperVolumeExplicit.h>
#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include "TestingUtils.h"
using namespace std;


typedef VTKM_DEFAULT_DEVICE_ADAPTER_TAG DeviceAdapter;

void createTransferFunction(vtkm::rendering::ColorTable &colorMap)
{
    colorMap.AddAlphaControlPoint(0.0f, 1.0f);
    colorMap.AddAlphaControlPoint(0.2f, .3f);
    colorMap.AddAlphaControlPoint(0.4f, .1f);
    colorMap.AddAlphaControlPoint(0.6f, .1f);
    colorMap.AddAlphaControlPoint(0.8f, .2f);
    colorMap.AddAlphaControlPoint(1.0f, .3f);

}



int main(int argc, char *argv[])
{
    vtkm::rendering::MapperVolumeExplicit sceneRenderer;
    vtkm::rendering::Camera camera = vtkm::rendering::Camera();
    char *filename;
    string fieldname = "pointvar";
    //char *outFilename = NULL;
    int width = 1024, height = 1024;
    parseCommandLineArgs(argc, argv, camera, filename, fieldname, width, height);
    
    try
    {
      vtkm::cont::DataSet data = GetDataSet(filename);


      const vtkm::cont::CoordinateSystem coords = data.GetCoordinateSystem();
    
   
    vtkm::Bounds coordsBounds; // Xmin,Xmax,Ymin..
    coordsBounds = coords.GetBounds(DeviceAdapter());
    //set up a default camera
    vtkm::Vec<vtkm::Float32,3> totalExtent;
    totalExtent[0] = vtkm::Float32(coordsBounds.X.Max - coordsBounds.X.Min);
    totalExtent[1] = vtkm::Float32(coordsBounds.Y.Max - coordsBounds.Y.Min);
    totalExtent[2] = vtkm::Float32(coordsBounds.Z.Max - coordsBounds.Z.Min);
    vtkm::Float32 mag = vtkm::Magnitude(totalExtent);
    std::cout<<"Magnitude "<<mag<<" Extent "<<totalExtent<<std::endl;
    //std::cout<<" Bounds  "<<coordsBounds[0]<<","<<coordsBounds[2]<<","<<coordsBounds[4]<<","<<coordsBounds[1]<<","<<coordsBounds[3]<<","<<coordsBounds[5]<<","<<std::endl;
    vtkm::Normalize(totalExtent);
    camera.SetLookAt(totalExtent * (mag * .5f));
    vtkm::Vec<vtkm::Float32,3> up;
    up[0] = 1.f;
    up[1] = 0.f; 
    up[2] = 0.f;
    camera.SetViewUp(up);
    camera.SetClippingRange(1.f,100.f);
    camera.SetFieldOfView(60.f);
    vtkm::Vec<vtkm::Float32,3> defaultPos = -totalExtent * (mag *.9f);
    //vtkm::Vec<vtkm::Float32,3> defaultPos = totalExtent * (mag *1.5f);
    defaultPos[0] += .5;
    camera.SetPosition(defaultPos);
    //sceneRenderer.SetNumberOfSamples(500);
    data.PrintSummary(cout);
    cout<<"Using scalar field "<<fieldname<<endl;
    vtkm::cont::Field scalarField = data.GetField(fieldname);
    //vtkm::rendering::ColorTable colorTable("thermal");
    vtkm::rendering::ColorTable colorTable("cool2Warm");
    createTransferFunction(colorTable);
    vtkm::rendering::CanvasRayTracer surface(width, height);
    surface.SetBackgroundColor( vtkm::rendering::Color(1,1,1,1));
    surface.Clear();
    vtkm::rendering::Actor actor(data.GetCellSet(),coords,scalarField,colorTable);
    actor.Render(sceneRenderer, surface, camera);
    surface.SaveAs("Color.pnm");
    std::cout<<vtkm::rendering::DataToString();
    
    }
    catch (vtkm::cont::Error error) 
    {
      std::cout << "Got the unexpected error: " << error.GetMessage() << std::endl;
    }
    return 0;
}
