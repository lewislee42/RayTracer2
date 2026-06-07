# CMake file was originally copied from youtuber Erik Yuzwa (https://www.youtube.com/watch?v=_i4wRjcp8eU)

cmake_minimum_required(VERSION 4.1)

project(Sim C CXX OBJC)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(METAL_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/default.metallib)


add_custom_command(
  OUTPUT ${METAL_LIBRARY}
  # 1. Compile .metal to intermediate .air file
  COMMAND xcrun -sdk macosx metal 
    -I ${CMAKE_CURRENT_SOURCE_DIR}/includes
    -c ${CMAKE_CURRENT_SOURCE_DIR}/srcs/Shaders/raytracer.metal
    -o ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air
  # 2. Link .air to final .metallib
  COMMAND xcrun -sdk macosx metallib 
    ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air
    -o ${METAL_LIBRARY}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/srcs/Shaders/raytracer.metal
  COMMENT "Compiling Metal shaders for ${CMAKE_SYSTEM_NAME}"
)


# Source files
set(SOURCES 
  ./srcs/main.cpp
  ./srcs/Engine.cpp
  ./srcs/DisplayWindow/DisplayWindow.cpp
  ./srcs/Camera.cpp
  ./srcs/Utils.cpp
  ./srcs/Shaders/raytracer.metal
)


set(IMGUI_SOURCES
  ./libs/imgui/imgui.cpp
  ./libs/imgui/imgui_draw.cpp
  ./libs/imgui/imgui_demo.cpp
  ./libs/imgui/imgui_widgets.cpp
  ./libs/imgui/imgui_tables.cpp
  ./libs/imgui/backends/imgui_impl_sdl3.cpp
  ./libs/imgui/backends/imgui_impl_sdlgpu3.cpp
)

include(FetchContent)
function(include_dependency libName gitUrl gitTag)
  FetchContent_Declare(${libName}
    GIT_REPOSITORY  ${gitUrl}
    GIT_TAG         ${gitTag}
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
  )
  FetchContent_MakeAvailable(${libName})
endFunction()

set(LIB2 entt)
find_package(${LIB2} QUIET)
if (NOT ${LIB2}_FOUND)
  include_dependency(${LIB2} https://github.com/skypjack/entt.git v3.15.0)
endif()

set(LIB3 SDL)
find_package(${LIB3} QUIET)
if (NOT ${LIB3}_FOUND)
  include_dependency(${LIB3} https://github.com/libsdl-org/SDL.git release-3.2.28)
endif()

FetchContent_Declare(metal_cpp
  GIT_REPOSITORY  https://github.com/bkaradzic/metal-cpp.git
  GIT_TAG         metal-cpp_26 
  GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(metal_cpp)
# message(STATUS "metal_cpp_SOURCE_DIR = ${metal_cpp_SOURCE_DIR}")



add_custom_target(MetalShaders DEPENDS ${METAL_LIBRARY})

add_executable(Sim MACOSX_BUNDLE ${SOURCES} ${IMGUI_SOURCES})

add_dependencies(Sim MetalShaders)



target_include_directories(Sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_include_directories(Sim PRIVATE ${metal_cpp_SOURCE_DIR})
target_include_directories(Sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)

target_link_libraries(Sim PRIVATE ${LIB1})
target_link_libraries(Sim PRIVATE SDL3::SDL3)
target_link_libraries(Sim PRIVATE EnTT::EnTT)

find_library(METAL_FRAMEWORK Metal REQUIRED)
find_library(QUARTZ_FRAMEWORK QuartzCore REQUIRED)
find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
target_link_libraries(Sim PRIVATE ${METAL_FRAMEWORK} ${QUARTZ_FRAMEWORK} ${APPKIT_FRAMEWORK})

target_compile_options(Sim PUBLIC -fsanitize=address -g -O1)
target_link_options(Sim PUBLIC -fsanitize=address)
