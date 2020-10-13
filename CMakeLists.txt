
cmake_minimum_required(VERSION 3.9)
project(tsar)

enable_testing()

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(test)
