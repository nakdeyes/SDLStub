cmake_minimum_required(VERSION 3.22.1)

# CMake file for compiling the imgui library
project(imgui)

# Add THIS Directory in the top level CMakeLists.txt file using
#    add_subdirectory(/home/jorge/code-new/imgui-demo/external/imgui/)

set (CMAKE_CXX_STANDARD 20)

# Add the library that is built on this subdirectory.
add_library(imgui SHARED 
imconfig.h
imgui_demo.cpp
imgui_draw.cpp
imgui_internal.h
imgui_tables.cpp
imgui_widgets.cpp
imgui.cpp
imgui.h
)

# target_include_directories(${PROJECT_NAME}.out PRIVATE Adder)
target_include_directories(imgui PRIVATE /home/jorge/code-new/include/imgui)
