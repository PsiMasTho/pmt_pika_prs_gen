file(GLOB SOURCES "*.cpp")

find_package(pmt CONFIG REQUIRED)

add_executable(pmt_pika_prs_gen_cli ${SOURCES})
target_link_libraries(pmt_pika_prs_gen_cli
  ${PROJECT_NAME}_builder
  ${PROJECT_NAME}_meta
  pmt
)

target_include_directories(pmt_pika_prs_gen_cli PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

install(TARGETS pmt_pika_prs_gen_cli
        EXPORT pmt_pika_prs_gen_cli-config
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT pmt_pika_prs_gen_cli-config
        FILE pmt_pika_prs_gen_cli-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pmt_pika_prs_gen_cli)
