cmake_minimum_required(VERSION 3.0.0)

project(ParsissCamera VERSION 0.1.0)


if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/std:c++17")
else()
  add_compile_options("-std=c++17")
endif()


include(CTest)
enable_testing()

find_package(VTK REQUIRED)

if(NOT VTK_FOUND)
  message(FATAL_ERROR "vtk is required to build this project")
endif()


configure_file(configuration.h.in "${CMAKE_CURRENT_SOURCE_DIR}/ParsissCameraConfiguration.h")

set(PUBLIC_INCLUDE_DIRS "Utils" "DataStructures" "${VTK_INCLUDE_DIRS}")

file(GLOB HEADERS "DataStructures/*.h" "DataStructures/*.hpp" "Utils/*.h" "*.h")
file(GLOB SOURCES "DataStructures/*.[ch]pp" "Utils/*.cpp" "*.cpp")

list(FILTER SOURCES EXCLUDE REGEX ".*SimulatedTest.cpp")

add_library(ParsissCameraLib STATIC ${SOURCES})

target_include_directories(ParsissCameraLib PUBLIC ${PUBLIC_INCLUDE_DIRS})
target_include_directories(ParsissCameraLib PRIVATE ${VTK_INCLUDE_DIRS})

target_link_libraries(ParsissCameraLib PUBLIC ${VTK_LIBRARIES})

add_executable(SimulatedTest SimulatedTest.cpp)
target_link_libraries(SimulatedTest ParsissCameraLib)
target_include_directories(SimulatedTest PUBLIC ${PUBLIC_INCLUDE_DIRS})

set_target_properties(
  ParsissCameraLib
  PROPERTIES
  PUBLIC_HEADER "${HEADERS}"
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Library")
install(
  TARGETS ParsissCameraLib SimulatedTest
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)

install(
  DIRECTORY "nuget/"
  DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
