file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_base_test ${SOURCES})
target_link_libraries(${PROJECT_NAME}_base_test ${PROJECT_NAME}_base)

target_include_directories(${PROJECT_NAME}_base_test PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(exe)