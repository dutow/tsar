
add_executable(tsar_test_typesort typesort_test.cxx)
target_link_libraries(tsar_test_typesort tsar)

add_executable(tsar_test_typewrap typewrap_test.cxx)
target_link_libraries(tsar_test_typewrap tsar)

add_executable(tsar_test_unit 
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
add_test(tsar_test_unit tsar_test_unit)
target_link_libraries(tsar_test_unit tsar)

target_compile_options(tsar_test_unit PUBLIC "-fsanitize=undefined")
target_link_options(tsar_test_unit PUBLIC "-fsanitize=undefined")

add_test(NAME field_failure1
  COMMAND ${CMAKE_CTEST_COMMAND}
    --build-and-test
      ${CMAKE_CURRENT_LIST_DIR}/field_failure
      ${CMAKE_CURRENT_BINARY_DIR}/field_failure
    --build-generator ${CMAKE_GENERATOR}
    --test-command ${CMAKE_CTEST_COMMAND}
    )
set_tests_properties(field_failure1 PROPERTIES WILL_FAIL TRUE)
