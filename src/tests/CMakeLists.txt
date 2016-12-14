##=============================================================================
##
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##
##  Copyright 2015 Sandia Corporation.
##  Copyright 2015 UT-Battelle, LLC.
##  Copyright 2015 Los Alamos National Security.
##
##  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
##  the U.S. Government retains certain rights in this software.
##  Under the terms of Contract DE-AC52-06NA25396 with Los Alamos National
##  Laboratory (LANL), the U.S. Government retains certain rights in
##  this software.
##
##=============================================================================

#Find the VTK-m package
find_package(VTKm REQUIRED QUIET
  OPTIONAL_COMPONENTS Serial TBB CUDA Rendering
  )

if(VTKm_CUDA_FOUND)
    cuda_include_directories(${VTKm_INCLUDE_DIRS}})
    cuda_add_executable(Volume testvolume.cu)
else()
  add_executable(Volume testvolume.cxx)
endif()

target_include_directories(Volume PRIVATE ${VTKm_INCLUDE_DIRS})
target_link_libraries(Volume  ${VTKm_LIBRARIES})
target_compile_options(Volume PRIVATE ${VTKm_COMPILE_OPTIONS})
