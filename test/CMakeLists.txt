
ADD_EXECUTABLE(test_tsar typesort_test.cxx)

#ADD_EXECUTABLE(test_standard_storage standard_storage_test.cxx)

ADD_EXECUTABLE(test_unit standard_tuple_test.cxx unit_main.cxx context_aware_tuple_test.cxx)
TARGET_INCLUDE_DIRECTORIES(test_unit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Catch2/single_include/catch2")
ADD_TEST(test_unit test_unit)
