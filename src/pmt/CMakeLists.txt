file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE .)

add_subdirectory(base)
add_subdirectory(parserbuilder)
add_subdirectory(util)
