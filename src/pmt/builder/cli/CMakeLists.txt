file(GLOB SOURCES "*.cpp")

add_executable(pika_parser_generator_cli ${SOURCES})
target_link_libraries(pika_parser_generator_cli ${PROJECT_NAME}_builder)

target_include_directories(pika_parser_generator_cli PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

install(TARGETS pika_parser_generator_cli
        EXPORT pika_parser_generator_cli-config
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT pika_parser_generator_cli-config
        FILE pika_parser_generator_cli-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pika_parser_generator_cli)