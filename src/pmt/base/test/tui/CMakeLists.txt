file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME}_base_test_tui ${SOURCES})
target_link_libraries(${PROJECT_NAME}_base_test_tui ${PROJECT_NAME}_base_test)

target_include_directories(${PROJECT_NAME}_base_test_tui PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)