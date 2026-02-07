file(GLOB SOURCES "*.cpp")

add_executable(example_1 ${SOURCES})
target_link_libraries(example_1 pmt_pika_rt)