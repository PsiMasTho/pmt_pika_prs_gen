file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME}_base_test_exe ${SOURCES})
target_link_libraries(${PROJECT_NAME}_base_test_exe ${PROJECT_NAME}_base_test)

target_include_directories(${PROJECT_NAME}_base_test_exe PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)