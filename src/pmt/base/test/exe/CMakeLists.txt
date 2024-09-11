file(GLOB SOURCES "*.c")

add_executable(pmt_base_test_exe ${SOURCES})

target_link_libraries(pmt_base_test_exe pmt_base_test)
