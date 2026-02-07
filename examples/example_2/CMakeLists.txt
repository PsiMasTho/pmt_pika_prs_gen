file(GLOB SOURCES "*.cpp")

add_executable(example_2 ${SOURCES})
target_link_libraries(example_2 pmt_pika_rt)