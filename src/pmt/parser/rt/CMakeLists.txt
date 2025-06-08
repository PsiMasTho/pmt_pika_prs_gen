file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_parser_rt ${SOURCES})
target_link_libraries(${PROJECT_NAME}_parser_rt ${PROJECT_NAME}_base ${PROJECT_NAME}_util_sm ${PROJECT_NAME}_parser)

target_include_directories(${PROJECT_NAME}_parser_rt PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
