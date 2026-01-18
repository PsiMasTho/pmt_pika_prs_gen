file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME}_builder_tui ${SOURCES})
target_link_libraries(${PROJECT_NAME}_builder_tui ${PROJECT_NAME}_builder)

target_include_directories(${PROJECT_NAME}_builder_tui PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

install(TARGETS ${PROJECT_NAME}_builder_tui
        EXPORT ${PROJECT_NAME}_builder_tui-config
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${PROJECT_NAME}_builder_tui-config
        FILE ${PROJECT_NAME}_builder_tui-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}_builder_tui)