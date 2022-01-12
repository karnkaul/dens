cmake_minimum_required(VERSION 3.14)
enable_language(CXX)

set(project_name dens)
set(${project_name}_version 1.0.1)
set(cxx_standard cxx_std_20)

project(${project_name} VERSION ${${project_name}_version})
set(is_root_project OFF)	# indicate if this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(is_root_project ON)
  message(STATUS "${PROJECT_NAME} is top level")
endif()

option(DENS_BUILD_TESTS "Build dens tests" ${is_root_project})
option(DENS_INSTALL ${is_root_project})

# cmake-utils
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  cmake-utils
  GIT_REPOSITORY https://github.com/karnkaul/cmake-utils
  GIT_TAG v1.0.1
)
FetchContent_MakeAvailable(cmake-utils)
FetchContent_GetProperties(cmake-utils)

set(${PROJECT_NAME}_soversion ${PROJECT_VERSION_MAJOR})
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${${PROJECT_NAME}_version})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION ${${PROJECT_NAME}_soversion})
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_${PROJECT_NAME}_MAJOR_VERSION ${${PROJECT_NAME}_soversion})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${PROJECT_NAME}_MAJOR_VERSION)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_sources(${PROJECT_NAME} PRIVATE
  include/dens/detail/archetype.hpp
  include/dens/detail/sign.hpp
  include/dens/detail/tarray.hpp
  include/dens/entity.hpp
  include/dens/registry.hpp
  include/dens/system_group.hpp
  include/dens/system.hpp
)
get_target_property(sources ${PROJECT_NAME} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

if(DENS_INSTALL)
  include("${cmake-utils_SOURCE_DIR}/cmake-utils.cmake")
  install_and_export_target(TARGET ${PROJECT_NAME})
endif()

if(DENS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
