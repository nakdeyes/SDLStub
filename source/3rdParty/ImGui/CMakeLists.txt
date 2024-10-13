cmake_minimum_required(VERSION 3.22.1)

# CMake file for compiling the imgui library
project(imgui)

# Add THIS Directory in the top level CMakeLists.txt file using
#    add_subdirectory(/home/jorge/code-new/imgui-demo/external/imgui/)

# Add the library that is built on this subdirectory.
add_library(imgui SHARED 
imgui.cpp
imgui.h
imgui_internal.h
)

# target_include_directories(${PROJECT_NAME}.out PRIVATE Adder)
target_include_directories(imgui PRIVATE /home/jorge/code-new/include/imgui)
