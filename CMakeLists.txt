
cmake_minimum_required(VERSION 2.8)
project(tsar)

ENABLE_TESTING()

SET(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(test)
