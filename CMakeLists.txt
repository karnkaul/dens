cmake_minimum_required(VERSION 3.14)
enable_language(CXX)

set(DENS_VERSION "0.1" CACHE STRING "dens version" FORCE)
project(dens VERSION ${DENS_VERSION})

set(is_root_project OFF)	# indicate if this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(is_root_project ON)
  message(STATUS "${PROJECT_NAME} is top level")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml.in" "${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml")

option(DENS_BUILD_TESTS "Build dens tests" ${is_root_project})

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE include)
add_subdirectory(include/dens)

get_target_property(sources ${PROJECT_NAME} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

if(DENS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
