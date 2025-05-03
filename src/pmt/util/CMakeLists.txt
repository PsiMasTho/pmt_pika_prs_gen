file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_util ${SOURCES})
target_link_libraries(${PROJECT_NAME}_util ${PROJECT_NAME}_base)

target_include_directories(${PROJECT_NAME}_util PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(smrt)
add_subdirectory(smct)
