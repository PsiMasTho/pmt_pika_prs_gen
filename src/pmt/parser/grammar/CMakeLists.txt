file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_parser_grammar ${SOURCES})
target_link_libraries(${PROJECT_NAME}_parser_grammar ${PROJECT_NAME}_base ${PROJECT_NAME}_sm ${PROJECT_NAME}_parser)

target_include_directories(${PROJECT_NAME}_parser_grammar PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)