cmake_minimum_required(VERSION 3.16)
project(3way-openmp)

find_package(OpenMP REQUIRED)

# Create a library for the file_io.c
add_library(file_io src/file_io.c)
# Allow file_io to find its includes
target_include_directories(file_io PUBLIC include)

# Compile the main executable
add_executable(${PROJECT_NAME} src/max_ascii_openmp.c)

# Allow the executable to find its includes
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link OpenMP
if(OpenMP_C_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_C)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE file_io)

# Use c11 standard
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)
target_compile_features(file_io PUBLIC c_std_11)
# Use -Wall -Wextra flags
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_compile_options(file_io PRIVATE -Wall -Wextra)