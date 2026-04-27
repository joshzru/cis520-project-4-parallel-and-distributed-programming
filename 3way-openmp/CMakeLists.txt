cmake_minimum_required(VERSION 3.16)
project(3way-openmp)

# Create a library for the file_io.c
add_library(file_io src/file_io.c)
# Allow file_io to find its includes
target_include_directories(file_io PUBLIC include)

# Compile the main executable
add_executable(${PROJECT_NAME} src/max_ascii_openmp.c)

# Use c11 standard
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)
# Use -Wall -Wextra flags
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# Link our executable and file_io library
target_link_libraries(${PROJECT_NAME} PRIVATE file_io openmp)