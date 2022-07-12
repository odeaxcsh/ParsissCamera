cmake_minimum_required(VERSION 3.0.0)
project(GraphTest VERSION 0.1.0)

set(CXX_STANDARD "c++17")
add_compile_options("/std:c++17")

include(CTest)
enable_testing()

find_package(VTK REQUIRED)

if(NOT VTK_FOUND)
  message(FATAL_ERROR "vtk is required to build this project")
endif()


configure_file(configuration.h.in configuration.h)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB DSSRC "DataStructures/*.[hc]pp")
file(GLOB SOURCES "*.cpp")

add_executable(GraphTest main.cpp ${DSSRC} ${SOURCES} "Utils/StringUtils.cpp")


target_include_directories(GraphTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/DataStructures")
target_include_directories(GraphTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Utils")
target_include_directories(GraphTest PUBLIC "${VTK_INCLUDE_DIRS}")

target_link_libraries(GraphTest PUBLIC ${VTK_LIBRARIES})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
