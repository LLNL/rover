#######
# using intel@14.0.3 compiler spec
#######

# c compiler used by spack
set(CMAKE_C_COMPILER "/usr/local/bin/icc" CACHE PATH "")

# cpp compiler used by spack
set(CMAKE_CXX_COMPILER "/usr/local/bin/icpc" CACHE PATH "")

# fortran compiler used by spack
set(CMAKE_Fortran_COMPILER  "/usr/local/bin/ifort" CACHE PATH "")

# MPI Support
set(ENABLE_MPI  ON CACHE PATH "")

set(MPI_C_COMPILER  "/usr/local/tools/mvapich2-intel-2.0/bin/mpicc" CACHE PATH "")

set(MPI_CXX_COMPILER "/usr/local/tools/mvapich2-intel-2.0/bin/mpicc" CACHE PATH "")

set(MPI_Fortran_COMPILER "/usr/local/tools/mvapich2-intel-2.0/bin/mpif90" CACHE PATH "")

set(MPIEXEC /usr/bin/srun CACHE PATH "")

set(MPIEXEC_NUMPROC_FLAG -n CACHE PATH "")



# CUDA support
#set(ENABLE_CUDA OFF CACHE PATH "")
set(ENABLE_CUDA OFF CACHE PATH "")

#set(CUDA_BIN_DIR /opt/cudatoolkit-7.0/bin CACHE PATH "")

# sphinx from uberenv
# not built ...
# conduit from uberenv
set(CONDUIT_DIR "/nfs/tmp2/larsen30/conduit/install" CACHE PATH "")

set(TBB_DIR "/usr/gapps/visit/strawman/uberenv_libs/spack/opt/spack/chaos_5_x86_64_ib/intel-14.0.3/tbb-4.4.3-al6fuqhyuhr6ju4daik3mfwk5j7gcyvw" CACHE PATH "")

# vtkm from uberenv
set(VTKM_DIR "/nfs/tmp2/larsen30/vtk-m/install" CACHE PATH "")

