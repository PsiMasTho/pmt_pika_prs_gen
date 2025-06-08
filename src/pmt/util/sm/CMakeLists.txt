add_library(${PROJECT_NAME}_util_sm INTERFACE)

target_include_directories(${PROJECT_NAME}_util_sm INTERFACE .)

add_subdirectory(ct)
