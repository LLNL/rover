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
    const vtkm::Int32 numOpacityPoints = 256;
    const vtkm::Int32 numPegPoints = 8;

//    unsigned char charOpacity[numOpacityPoints] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,6,40,40,40,40,24,24,9,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,24,24,24,24,24,38,66,83,118,118,118,118,128,128,135,145,145,145,145,145,255,255,255,255,104,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,220,220,220,220,220,220,220,220,223,227,230,234,234,237,241,244,248,251,251,255,255 };
    unsigned char charOpacity[numOpacityPoints] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,6,40,40,40,40,24,24,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,24,24,24,24,24,24,38,66,83,118,118,118,118,128,128,135,145,145,145,145,145,255,255,255,255,104,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,220,220,220,220,220,220,220,220,223,227,230,234,234,237,241,244,248,251,251,255,255 };

    for (int i = 0; i < numOpacityPoints; ++i)
    {
        vtkm::Float32 position = vtkm::Float32(i) / vtkm::Float32(numOpacityPoints);
        vtkm::Float32 value = vtkm::Float32(charOpacity[i] / vtkm::Float32(255));
        value =0.02f;
        colorMap.AddAlphaControlPoint(position, value);
    }

    unsigned char controlPointColors[numPegPoints*3] = { 
           128, 0, 128, 
           0, 128, 128,
           128, 128, 0, 
           128, 128, 128, 
           255, 255, 0, 
           255, 96, 0, 
           107, 0, 0, 
           224, 76, 76 
       };

    vtkm::Float32 controlPointPositions[numPegPoints] = { 0.f, 0.543f, 0.685f, 0.729f, 0.771f, 0.804f, 0.857f, 1.0f };

    for (int i = 0; i < numPegPoints; ++i)
    {
        vtkm::Float32 position = vtkm::Float32(i) / vtkm::Float32(numOpacityPoints);
        vtkm::rendering::Color color;
        color.Components[0] = vtkm::Float32(controlPointColors[i*3+0] / vtkm::Float32(255));
        color.Components[1] = vtkm::Float32(controlPointColors[i*3+1] / vtkm::Float32(255));
        color.Components[2] = vtkm::Float32(controlPointColors[i*3+2] / vtkm::Float32(255));
        colorMap.AddControlPoint(controlPointPositions[i], color);
    }

}



int main(int argc, char *argv[])
{
    vtkm::rendering::MapperVolumeExplicit<DeviceAdapter> sceneRenderer;
    vtkm::rendering::Camera camera = vtkm::rendering::Camera(vtkm::rendering::Camera::VIEW_3D);
    char *filename;
    string fieldname = "pointvar";
    //char *outFilename = NULL;
    parseCommandLineArgs(argc, argv, camera, filename, fieldname);
    
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
    camera.Camera3d.LookAt = totalExtent * (mag * .5f);
    vtkm::Vec<vtkm::Float32,3> up;
    up[0] = 1.f;
    up[1] = 0.f; 
    up[2] = 0.f;
    camera.Camera3d.Up = up;
    camera.NearPlane = 1.f;
    camera.FarPlane = 100.f;
    camera.Camera3d.FieldOfView = 60.f;
    vtkm::Vec<vtkm::Float32,3> defaultPos = -totalExtent * (mag *.9f);
    camera.Camera3d.Position = camera.Camera3d.LookAt;
    //camera.Camera3d.Position[1] = defaultPos[1]*1.3;
    camera.Camera3d.Position = defaultPos;
    //camera.Camera3d.Position[1] = 1.3;
    //camera.Camera3d.Position[0] += .2; //camera.Position[0];
    //camera.Camera3d.Position[1] = .2;//camera.Position[0];
    //camera.Camera3d.Position[2] = .2;//camera.Position[0];
    camera.Height = 1024;
    camera.Width = 1024;
    //camera.Position[1] +=2;
    vtkm::Vec<vtkm::Float32,3> vdir = camera.Camera3d.Position - camera.Camera3d.LookAt;
    std::cout<<"Look dir "<<vdir<<std::endl;
    vdir = camera.Camera3d.LookAt - camera.Camera3d.Position;
    vtkm::Normalize(vdir);
    std::cout<<"Look dir oposite"<<vdir<<std::endl;
    std::cout<<"Camera pos "<<camera.Camera3d.Position<<std::endl;
    std::cout<<"lookAt "<<camera.Camera3d.LookAt<<std::endl;

    
    //sceneRenderer.SetNumberOfSamples(500);
    data.PrintSummary(cout);
    cout<<"Using scalar field "<<fieldname<<endl;
    vtkm::cont::Field scalarField = data.GetField(fieldname);
    //vtkm::rendering::ColorTable colorTable("thermal");
    vtkm::rendering::ColorTable colorTable;
    createTransferFunction(colorTable);
    vtkm::rendering::CanvasRayTracer surface(camera.Width,camera.Height);
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
