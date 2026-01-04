file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_sm ${SOURCES})
target_link_libraries(${PROJECT_NAME}_sm ${PROJECT_NAME}_base ${PROJECT_NAME}_util)

target_include_directories(${PROJECT_NAME}_sm PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
