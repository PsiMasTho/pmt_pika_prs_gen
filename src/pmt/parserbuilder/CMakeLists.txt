file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_parserbuilder ${SOURCES})
target_link_libraries(${PROJECT_NAME}_parserbuilder ${PROJECT_NAME}_util_smct)
target_link_libraries(${PROJECT_NAME}_parserbuilder ${PROJECT_NAME}_util_smrt)
target_link_libraries(${PROJECT_NAME}_parserbuilder ${PROJECT_NAME}_base)

target_include_directories(${PROJECT_NAME}_parserbuilder PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(exe)