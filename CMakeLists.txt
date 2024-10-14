###############################################################################
####  CMAKE SETUP
cmake_minimum_required(VERSION 3.16)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
# set(CMAKE_C_COMPILER_FORCED TRUE)
# set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

###############################################################################
####  PROJECT SETUP
project(SDLStub)

# Set the name of the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Create an executable or a shared library based on the platform and add our sources to it
if (ANDROID)
	# The SDL java code is hardcoded to load libmain.so on android, so we need to change EXECUTABLE_NAME
	set(EXECUTABLE_NAME main)
	add_library(${EXECUTABLE_NAME} SHARED)
else()
	add_executable(${EXECUTABLE_NAME})
endif()

# Add your sources to the target
target_sources(${EXECUTABLE_NAME} 
PRIVATE 
    source/app/main.cpp
    source/app/SDLApp.cpp
    source/app/SDLApp.h
    source/app/platform/ios/iosLaunchScreen.storyboard
)
# What is iosLaunchScreen.storyboard? This file describes what Apple's mobile platforms
# should show the user while the application is starting up. If you don't include one,
# then you get placed in a compatibility mode that does not allow HighDPI.
# This file is referenced inside Info.plist.in, where it is marked as the launch screen file.
# It is also ignored on non-Apple platforms. 

# To get an app icon on Apple platforms, add it to your executable.
# Afterward, the image file in Info.plist.in.
if(APPLE)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "content/platform/logo.png")
endif()

# Set C++ version
set (CMAKE_CXX_STANDARD 20)
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_20)

# on Web targets, we need CMake to generate a HTML webpage. 
if(CMAKE_SYSTEM_NAME MATCHES Emscripten)
	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

###############################################################################
####  Fetch 3rdParties

include(FetchContent)

set(SDL_OPENGL ON)
set(SDL_VIDEO ON)
set(BUILD_SHARED_LIBS OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)

set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/source/3rdParty/ImGui)
set(SDL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/source/3rdParty/SDL3)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        "origin/docking"
  SOURCE_DIR     ${IMGUI_PATH}
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   ON
  SYSTEM
)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        e292d1f5ace469f718d7b6b4dec8c28e37dcaa0e # preview-3.1.3
  SOURCE_DIR     ${SDL_PATH}
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   ON
  SYSTEM
)

FetchContent_MakeAvailable(imgui SDL3)

#message(SDL3_SOURCE_DIR="${SDL3_SOURCE_DIR}")
#message(imgui_SOURCE_DIR="${imgui_SOURCE_DIR}")

###############################################################################
####  ImGui

# Imgui doesn't come with a CMakeLists so easiest is to just create a lib here.
add_library(imgui STATIC
    ${IMGUI_PATH}/imconfig.h
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_internal.h
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui.h
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
    ${IMGUI_PATH}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdl3.h
)

# configure ImGui
include_directories(${IMGUI_PATH})
target_link_libraries(${EXECUTABLE_NAME} PUBLIC imgui)

###############################################################################
####  SDL

# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
#add_subdirectory(${SDL_SOURCE_DIR} EXCLUDE_FROM_ALL)

include_directories(${SDL_PATH}/include)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3)
#target_compile_definitions(${EXECUTABLE_NAME} INTERFACE SDL_MAIN_USE_CALLBACKS)

###############################################################################
####  OpenGL

find_package(OpenGL REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC OpenGL::GL)

###############################################################################
####  Final CMAKE packaging and executable stuff

# set some extra configs for each platform
set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    # On macOS, make a proper .app bundle instead of a bare executable
    MACOSX_BUNDLE TRUE
    # Set the Info.plist file for Apple Mobile platforms. Without this file, your app
    # will not launch. 
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/source/app/platform/macos/Info.plist.in"

    # in Xcode, create a Scheme in the schemes dropdown for the app.
    XCODE_GENERATE_SCHEME TRUE
    # Identification for Xcode
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
)

# on Visual Studio, set our app as the default project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

# On macOS Platforms, ensure that the bundle is valid for distribution by calling fixup_bundle.
# note that fixup_bundle does not work on iOS, so you will want to use static libraries 
# or manually copy dylibs and set rpaths
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    INSTALL(CODE 
        "include(BundleUtilities)
        fixup_bundle(\"$<TARGET_BUNDLE_DIR:${EXECUTABLE_NAME}>\" \"\" \"\")
        " 
        COMPONENT Runtime
    )
endif()
install(TARGETS ${EXECUTABLE_NAME} BUNDLE DESTINATION ./install)

