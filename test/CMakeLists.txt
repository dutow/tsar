
ADD_EXECUTABLE(test_typesort typesort_test.cxx)

ADD_EXECUTABLE(test_typewrap typewrap_test.cxx)

ADD_EXECUTABLE(test_unit 
  #cat_test.cxx
  #standard_tuple_test.cxx
  unit_main.cxx
  #context_aware_tuple_test.cxx
  #observable_test.cxx
  packed_ptr_test.cxx
  log_search_test.cxx
  list_test.cxx
  cts_test.cxx
  field_test.cxx
)
ADD_TEST(test_unit test_unit)
