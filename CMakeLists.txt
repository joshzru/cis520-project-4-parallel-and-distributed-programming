cmake_minimum_required(VERSION 3.16)
project(project-4)

# Tell cmake to search through each subdirectory for a CMakeLists.txt
add_subdirectory(3way-mpi)
add_subdirectory(3way-openmp)
add_subdirectory(3way-pthreads)