file(GLOB SOURCES "*.cpp")

add_executable(example ${SOURCES})
target_link_libraries(example pmt_pika_rt)