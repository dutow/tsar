
add_executable(test_typesort typesort_test.cxx)
target_link_libraries(test_typesort tsar)

add_executable(test_typewrap typewrap_test.cxx)
target_link_libraries(test_typewrap tsar)

add_executable(test_unit 
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
add_test(test_unit test_unit)
target_link_libraries(test_unit tsar)

target_compile_options(test_unit PUBLIC "-fsanitize=undefined")
target_link_options(test_unit PUBLIC "-fsanitize=undefined")
