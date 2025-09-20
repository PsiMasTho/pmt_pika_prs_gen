file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_parser_bytecode ${SOURCES})
target_link_libraries(${PROJECT_NAME}_parser_bytecode ${PROJECT_NAME}_parser ${PROJECT_NAME}_base)

target_include_directories(${PROJECT_NAME}_parser_bytecode PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)