file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_base ${SOURCES})
target_link_libraries(${PROJECT_NAME}_base ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}_base PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(test)
