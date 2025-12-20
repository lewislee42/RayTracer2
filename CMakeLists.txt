# CMake file was originally copied from youtuber Erik Yuzwa (https://www.youtube.com/watch?v=_i4wRjcp8eU)

cmake_minimum_required(VERSION 4.1)

project(Sim CXX OBJC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set (CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Source files
set(SOURCES 
  ./srcs/main.cpp
  ./srcs/Scene.cpp
  ./srcs/InfoWindow/InfoWindow.cpp
  ./srcs/DisplayWindow/DisplayWindow.cpp
)

set(IMGUI_SOURCES
  ./libs/imgui/imgui.cpp
  ./libs/imgui/imgui_draw.cpp
  ./libs/imgui/imgui_demo.cpp
  ./libs/imgui/imgui_widgets.cpp
  ./libs/imgui/imgui_tables.cpp
  ./libs/imgui/backends/imgui_impl_glfw.cpp
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

set(LIB1 raylib)
find_package(${LIB1} QUIET)
if (NOT ${LIB1}_FOUND)
  include_dependency(${LIB1} https://github.com/raysan5/raylib.git 5.0)
endif()

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
message(STATUS "metal_cpp_SOURCE_DIR = ${metal_cpp_SOURCE_DIR}")



add_executable(Sim ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(Sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_include_directories(Sim PRIVATE ${raylib_INCLUDE_DIRS})
target_include_directories(Sim PRIVATE ${metal_cpp_SOURCE_DIR})
target_include_directories(Sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)

target_link_libraries(Sim PRIVATE ${LIB1})
target_link_libraries(Sim PRIVATE SDL3::SDL3)
target_link_libraries(Sim PRIVATE EnTT::EnTT)
