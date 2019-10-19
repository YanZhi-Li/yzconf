cmake_minimum_required(VERSION 3.0.0 )
project(yzconf VERSION 0.1.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -ggdb -std=c++11 -Wall -Werror -Wno-unused-function")
SET(LIB_SRC 
    yzconf/log.cc
    yzconf/util.cc)
ADD_LIBRARY(yzconf SHARED ${LIB_SRC})

add_executable(a.out tests/test.cc)
add_dependencies(a.out yzconf)
target_link_libraries(a.out yzconf)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
