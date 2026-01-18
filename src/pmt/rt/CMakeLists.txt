file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_rt ${SOURCES})

target_include_directories(${PROJECT_NAME}_rt PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
