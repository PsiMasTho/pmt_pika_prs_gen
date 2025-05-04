file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME}_parserbuilder_tui ${SOURCES})
target_link_libraries(${PROJECT_NAME}_parserbuilder_tui ${PROJECT_NAME}_parserbuilder)

target_include_directories(${PROJECT_NAME}_parserbuilder_tui PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)