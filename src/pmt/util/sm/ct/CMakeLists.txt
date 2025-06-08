file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_util_sm_ct ${SOURCES})
target_link_libraries(${PROJECT_NAME}_util_sm_ct ${PROJECT_NAME}_util)

target_include_directories(${PROJECT_NAME}_util_sm_ct PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
