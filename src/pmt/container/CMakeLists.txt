file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_container ${SOURCES})
target_link_libraries(${PROJECT_NAME}_container ${PROJECT_NAME}_util)

target_include_directories(${PROJECT_NAME}_container PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)