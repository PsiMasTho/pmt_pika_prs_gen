file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_util_smct ${SOURCES})
target_link_libraries(${PROJECT_NAME}_util_smct ${PROJECT_NAME}_util)

target_include_directories(${PROJECT_NAME}_util_smct PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
