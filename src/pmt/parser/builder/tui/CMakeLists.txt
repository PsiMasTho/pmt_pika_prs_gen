file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME}_parser_builder_tui ${SOURCES})
target_link_libraries(${PROJECT_NAME}_parser_builder_tui ${PROJECT_NAME}_parser_builder)

target_include_directories(${PROJECT_NAME}_parser_builder_tui PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)