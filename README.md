# How to compile

Load modules if needed

```
module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0 CUDA/11.7.0
```

Navigate to the program directory (3way-mpi, 3way-pthread, 3way-openmp)

Create the build directory

```
mkdir build
```

Navigate into the build directory and run cmake, then run make.