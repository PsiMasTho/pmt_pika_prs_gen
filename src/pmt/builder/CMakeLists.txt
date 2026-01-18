file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_builder ${SOURCES})
target_link_libraries(${PROJECT_NAME}_builder ${PROJECT_NAME}_container ${PROJECT_NAME}_meta ${PROJECT_NAME}_rt ${PROJECT_NAME}_util)

target_include_directories(${PROJECT_NAME}_builder PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(tui)