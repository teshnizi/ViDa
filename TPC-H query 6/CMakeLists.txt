cmake_minimum_required(VERSION 3.10)
project(cmake-build-debug/CMakeFiles/ViDaG)

set(CMAKE_CXX_STANDARD 11)

add_executable(ViDaG main.cpp)
add_executable(CSV_gen CSV_generator.cpp)
add_executable(file_editor file_editor.cpp)
add_executable(simple_random simple_random_generation.cpp)