
cmake_minimum_required(VERSION 3.18)
project(tsar)

enable_testing()

add_library(tsar INTERFACE)
target_include_directories(tsar INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(tsar INTERFACE "-Wno-invalid-offsetof")
target_compile_features(tsar INTERFACE cxx_std_20)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
