cmake_minimum_required(VERSION 3.18)
project(tsar_failure)

add_library(tsar INTERFACE)
target_include_directories(tsar INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_compile_options(tsar INTERFACE "-Wno-invalid-offsetof")
target_compile_features(tsar INTERFACE cxx_std_20)

add_executable(tsar_fail1 constructor_decltype_a.cxx)
target_link_libraries(tsar_fail1 tsar)
