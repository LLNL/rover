#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <vtkm/rendering/Camera.h>
#include <vtkm/io/reader/VTKDataSetReader.h>
using namespace std;

void printUsage()
{
  cerr<<"You didn't use me correctly or I was not programmed correctly."<<endl;
  cerr<<"     -f    OBJ/VTK filename for mesh.       Example : -f conference.obj"<<endl;
  cerr<<"     -fld  field name on mesh               Example : -fld myScalarField"<<endl;
  cerr<<"     -cp   Camera Position ( x y z ).       Example : -cp 1.2 0 -10.2"<<endl;
  cerr<<"     -cu   Camera Up Vector.                Example : -cu 0 1 0"<<endl;
  cerr<<"     -cla  Camera Look At Pos               Example : -cla 0 0 0"<<endl;
  cerr<<"     -fovx Field of view X                  Example : -fovx 45.0"<<endl;
  cerr<<"     -res  Resolution height width          Example : -res 1080 1920"<<endl;
  exit(1);
}

void parseCommandLineArgs(int argc, 
                          char *argv[],
                          vtkm::rendering::Camera &view, 
                          char *&filename,
                          string &fieldname,
                          int &width,
                          int &height)
{
  cout<<"Parse Args\n";
    if(argc<2)
    {
        cerr<<"Must specify a file to load."<<endl;
        printUsage();
    }

    for(int i=1; i<argc;i++)
    {
        if(strcmp (argv[i],"-f")==0)
        {   
            if(argc<=i) 
            {
                cerr<<"No file name specified."<<endl;
                printUsage();
            }
            filename=argv[++i];

        }
        else if(strcmp (argv[i],"-fld")==0)
        {   
            if(argc<=i) 
            {
                cerr<<"No field name specified."<<endl;
                printUsage();
            }
            fieldname=argv[++i];

        }
        else if(strcmp (argv[i],"-cp")==0)
        {
            if(argc<=i+3) 
            {
                cerr<<"Not enough input for camera coordinates."<<endl;
                printUsage();
            }
            float x=0; x=atof(argv[++i]);
            float y=0; y=atof(argv[++i]);
            float z=0; z=atof(argv[++i]);
            vtkm::Vec<vtkm::Float32,3> position(x,y,z);
            view.SetPosition(position);
        }
        else if(strcmp (argv[i],"-cu")==0)
        {
            if(argc<=i+3) 
            {
                cerr<<"Not enough input for up vector."<<endl;
                printUsage();
            }
            float x=0; x=atof(argv[++i]);
            float y=0; y=atof(argv[++i]);
            float z=0; z=atof(argv[++i]);
            vtkm::Vec<vtkm::Float32,3> up(x,y,z);
            view.SetViewUp(up);
            //tracer->camera->setCameraUp(x,y,z);
        }
        else if(strcmp (argv[i],"-cla")==0)
        {
            if(argc<=i+3) 
            {
                cerr<<"Not enough input for look at position."<<endl;
                printUsage();
            }
            float x=0; x=atof(argv[++i]);
            float y=0; y=atof(argv[++i]);
            float z=0; z=atof(argv[++i]);
            vtkm::Vec<vtkm::Float32,3> lookAt(x,y,z);
            view.SetLookAt(lookAt);
        }
        else if(strcmp (argv[i],"-fovx")==0)
        {
            if(argc<=i) 
            {
                cerr<<"Not enough input for FOV x."<<endl;
                printUsage();
            }
            float x=0; x=atof(argv[++i]);
            if(x==0)
            {
                cerr<<"Invalid FOV value "<<argv[i]<<endl;
                printUsage();
            }
            view.SetFieldOfView(x* 2.f);
        }
        else if(strcmp (argv[i],"-res")==0)
        {
            if(argc<=i+2) 
            {
                cerr<<"Not enough input for resolution."<<endl;
                printUsage();
            }
            float y=0; y=atoi(argv[++i]);
            float x=0; x=atoi(argv[++i]);
            if(x<1 || y<1)
            {
                cerr<<"Invalid resolution values. Must be non-zero integers."<<endl;
                printUsage();
            }
            width = x;
            height = y;
            
        }
        else
        {
            cerr<<"Unknown option : "<<argv[i]<<endl;
            printUsage();
        }
    }//input
}

// inline int fix_v(int index)  { return(index > 0 ? index - 1 : (index == 0 ? 0 : (int)v .size() + index)); }
// inline int fix_vt(int index) { return(index > 0 ? index - 1 : (index == 0 ? 0 : (int)vt.size() + index)); }
// inline int fix_vn(int index) { return(index > 0 ? index - 1 : (index == 0 ? 0 : (int)vn.size() + index)); }

// inline int getInt3(const char*& token)
// {
//     Vertex v(-1);
//     v.v = fix_v(atoi(token));
//     token += strcspn(token, "/ \t\r");
//     if (token[0] != '/') return(v);
//     token++;
    
//     // // it is i//n
//     // if (token[0] == '/') {
//     //     token++;
//     //     v.vn = fix_vn(atoi(token));
//     //     token += strcspn(token, " \t\r");
//     //     return(v);
//     // }
    
//     // // it is i/t/n or i/t
//     // v.vt = fix_vt(atoi(token));
//     // token += strcspn(token, "/ \t\r");
//     // if (token[0] != '/') return(v);
//     // token++;
    
//     // // it is i/t/n
//     // v.vn = fix_vn(atoi(token));
//     // token += strcspn(token, " \t\r");
//     return(v);
// }

void
ReadFile(const char *fname, float *&v, int &vS, int *&t, int &tS)
{
    ifstream ifile(fname);
    if (ifile.fail())
    {
        cerr << "Cannot open file " << fname << endl;
        exit(EXIT_FAILURE);
    }

    std::vector<float> verts;
    std::vector<int>   tris; 

    string line2;
    while (std::getline(ifile, line2))
    {
        const char *line = line2.c_str();
        if (strncmp(line, "v  ", strlen("v  ")) == 0)
        {
            float x, y, z;
            sscanf(line, "v  %f %f %f", &x, &y, &z);
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
        }
        else if (strncmp(line, "v ", strlen("v ")) == 0)
        {
            float x, y, z;
            sscanf(line, "v %f %f %f", &x, &y, &z);
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
        }

        if (strncmp(line, "f ", strlen("f ")) == 0)
        {
            char fields[3][30];

            int triangle[3];
            sscanf(line, "f %s %s %s", fields[0], fields[1], fields[2]);
            for(int i = 0; i < 3; i++)
            {
                string sdata(fields[i]); 
                std::size_t found = sdata.find("/");
                //cout<<"Line "<<line<<" "<<found<<endl;
                if(found != std::string::npos)
                {   
                    string indice = sdata.substr(0,found);
                    //cout<<"Has multiple vertex attributes "<<indice<<"\n";
                    triangle[i] = atoi(indice.c_str());
                    //exit(EXIT_FAILURE);
                }
                else triangle[i] = atoi(sdata.c_str());

            }
            //check to see if this face has normals or texture coords
            tris.push_back(triangle[0]);
            tris.push_back(triangle[1]);
            tris.push_back(triangle[2]);
        }
    }

    vS = verts.size();
    v = new float[vS];
    memcpy(v, &verts[0], vS*sizeof(float));
    vS /= 3; // 3 coordinates per vertex
    tS = tris.size();
    t = new int[tS];
    memcpy(t, &tris[0], tS*sizeof(float));
    tS /= 3; // 3 vertices per triangle
}



vtkm::cont::DataSet GetDataSet(const char *fname)
{
    cout<<"GetData\n";
    //Check the file extension
    string filename = fname;
    size_t dotPosition = filename.find_last_of(".");
    cout<<"Dot pos "<<dotPosition<<endl;
    if(dotPosition == string::npos)
    {
        cout<<"Error:  could not parse file extention of "<<filename<<endl;
        exit(1);
    }
    string fileExtention = filename.substr(dotPosition + 1);
    cout<<"File extention = "<<fileExtention<<endl;
    if(fileExtention != "obj" && fileExtention != "vtk") 
    {
        cout<<"Error: "<<fileExtention<<" not recognized\n";
        exit(1);
    }

    if(fileExtention == "vtk") 
    {
        cout<<"Reading vtk file "<<fname<<endl;
        vtkm::io::reader::VTKDataSetReader reader(fname);
        reader.PrintSummary(cout);
        vtkm::cont::DataSet dataset= reader.ReadDataSet();
        dataset.PrintSummary(cout);
        return dataset;
    }
    vtkm::cont::DataSet dataSet;

    float *verts    = NULL;
    int    numVerts = 0;
    int   *tris     = NULL;
    int    numTris  = 0;
    ReadFile(fname, verts, numVerts, tris, numTris);
    typedef vtkm::Vec<vtkm::Float32,3> CoordType;
    CoordType *coordinates = new CoordType[numVerts]; 
    vtkm::Float32 *vars= new vtkm::Float32[numVerts];
    cout<<"numVerts "<<numVerts<<"Num Tris "<<numTris<<endl;
    for (int i = 0; i < numVerts; ++i)
    {
        coordinates[i]=vtkm::make_Vec<vtkm::Float32>(verts[i*3+0],verts[i*3+1],verts[i*3+2]);
        vars[i]= float(i); //random scalar
    }
    dataSet.AddCoordinateSystem(
        vtkm::cont::CoordinateSystem("coordinates",coordinates, numVerts));
      //Set point scalar
    dataSet.AddField(vtkm::cont::Field("pointvar", vtkm::cont::Field::ASSOC_POINTS, vars, numVerts));

    //std::vector<vtkm::UInt8> shapes;
    //std::vector<vtkm::IdComponent> numindices;
    //std::vector<vtkm::Id> conn;

    vtkm::cont::CellSetExplicit<> cellSet(numVerts, "cells");
    cellSet.PrepareToAddCells(numTris, numTris * 3);
    //cellSet.AddCell(vtkm::CELL_SHAPE_TRIANGLE, 3, make_Vec<vtkm::Id>(0,1,2));
    //cellSet.AddCell(vtkm::CELL_SHAPE_QUAD, 4, make_Vec<vtkm::Id>(2,1,3,4)); 
    
    for (int i = 0; i < numTris; ++i)
    {
      cellSet.AddCell(vtkm::CELL_SHAPE_TRIANGLE, 3, vtkm::make_Vec<vtkm::Id>(tris[i*3+0]-1,tris[i*3+1]-1,tris[i*3+2]-1));
        // shapes.push_back(vtkm::CELL_SHAPE_TRIANGLE);
        // numindices.push_back(3);

        // conn.push_back(tris[i*3+0]);
        // conn.push_back(tris[i*3+1]);
        // conn.push_back(tris[i*3+2]);
    }
    cellSet.CompleteAddingCells();
    //cellSet.FillViaCopy(shapes, numindices, conn);
    dataSet.AddCellSet(cellSet);
    dataSet.PrintSummary(cout);
    return dataSet;
}
