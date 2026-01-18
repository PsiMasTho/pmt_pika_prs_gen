file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_util ${SOURCES})

target_include_directories(${PROJECT_NAME}_util PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

target_compile_definitions(${PROJECT_NAME}_util PUBLIC
PMT_SKEL_INSTALL_DIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/skel")
