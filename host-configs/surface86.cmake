#######
# using gcc compiler spec
#######

# c compiler used by spack
set(CMAKE_C_COMPILER "/usr/apps/gnu/4.9.3/bin/gcc" CACHE PATH "")

# cpp compiler used by spack
set(CMAKE_CXX_COMPILER "/usr/apps/gnu/4.9.3/bin/g++" CACHE PATH "")

# fortran compiler used by spack
set(ENABLE_FORTRAN ON CACHE PATH "")

# Enable python module builds
set(ENABLE_PYTHON ON CACHE INTERNAL PATH "")

# fortran compiler used by spack
set(CMAKE_Fortran_COMPILER  "/usr/apps/gnu/4.9.2/bin/mpigfortran" CACHE PATH "")

# OPENMP Support
set(ENABLE_OPENMP ON CACHE PATH "")

# MPI Support
set(ENABLE_MPI  ON CACHE PATH "")

set(MPI_C_COMPILER  "/usr/apps/gnu/4.9.2/bin/mpigcc" CACHE PATH "")

set(MPI_CXX_COMPILER "/usr/apps/gnu/4.9.2/bin/mpig++" CACHE PATH "")

set(MPI_Fortran_COMPILER "/usr/apps/gnu/4.9.2/bin/mpigfortran" CACHE PATH "")



# CUDA support
#set(ENABLE_CUDA OFF CACHE PATH "")
set(ENABLE_CUDA OFF CACHE PATH "")

#set(CUDA_BIN_DIR /opt/cudatoolkit-7.0/bin CACHE PATH "")

set(TBB_DIR "/usr/gapps/visit/strawman/uberenv_libs/spack/opt/spack/chaos_5_x86_64_ib/intel-14.0.3/tbb-4.4.3-al6fuqhyuhr6ju4daik3mfwk5j7gcyvw" CACHE PATH "")

# vtkm from uberenv
set(VTKM_DIR "/usr/workspace/wsb/larsen30/rover_dev/rover_vtkm/install" CACHE PATH "")

