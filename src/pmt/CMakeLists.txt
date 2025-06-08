add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE .)

add_subdirectory(base)
add_subdirectory(parser)
add_subdirectory(util)
