file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_parser ${SOURCES})

target_include_directories(${PROJECT_NAME}_parser PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(bytecode)
add_subdirectory(grammar)
add_subdirectory(rt)
add_subdirectory(builder)
