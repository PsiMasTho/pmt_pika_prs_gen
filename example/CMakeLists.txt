file(GLOB SOURCES "*.cpp")

add_executable(example ${SOURCES})
target_link_libraries(example pmt_ast pmt_pika_prs_gen_rt)