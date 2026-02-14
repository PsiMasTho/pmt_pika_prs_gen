file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME}_builder ${SOURCES})
target_link_libraries(${PROJECT_NAME}_builder
  PUBLIC
    ${PROJECT_NAME}_meta
    pmt
)

target_include_directories(${PROJECT_NAME}_builder PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

target_compile_definitions(${PROJECT_NAME}_builder PUBLIC
PMT_SKEL_INSTALL_DIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/skel")

target_compile_definitions(${PROJECT_NAME}_builder PUBLIC
$<BUILD_INTERFACE:PMT_SKEL_ROOT="${PROJECT_SOURCE_DIR}/skel">
$<INSTALL_INTERFACE:PMT_SKEL_ROOT="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/skel">)

add_subdirectory(cli)
