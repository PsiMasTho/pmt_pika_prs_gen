file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_parser_builder ${SOURCES})
target_link_libraries(${PROJECT_NAME}_parser_builder ${PROJECT_NAME}_base ${PROJECT_NAME}_util_sm ${PROJECT_NAME}_parser ${PROJECT_NAME}_parser_grammar ${PROJECT_NAME}_parser_rt)

target_include_directories(${PROJECT_NAME}_parser_builder PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(tui)