file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_util ${SOURCES})

target_include_directories(${PROJECT_NAME}_util PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)