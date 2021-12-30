# dtest
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  dtest
  GIT_REPOSITORY https://github.com/karnkaul/dtest
  GIT_TAG 0ca8c64
)
FetchContent_MakeAvailable(dtest)
FetchContent_GetProperties(dtest)

add_executable(${PROJECT_NAME}-test dens_test.cpp)
target_link_libraries(${PROJECT_NAME}-test dens::dens dtest::main)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME}-test PRIVATE -Wextra -Wall -Werror=return-type $<$<NOT:$<CONFIG:Debug>>:-Werror>)
endif()

add_test(${PROJECT_NAME}-test ${PROJECT_NAME}-test)
