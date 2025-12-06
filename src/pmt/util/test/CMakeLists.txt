file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_util_test ${SOURCES})
target_link_libraries(${PROJECT_NAME}_util_test ${PROJECT_NAME}_util)

target_include_directories(${PROJECT_NAME}_util_test PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(tui)
