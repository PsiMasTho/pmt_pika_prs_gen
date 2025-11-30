file(GLOB SOURCES "*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavformat
    libavcodec
    libavutil
    libavfilter
    libswscale
)

add_executable(${PROJECT_NAME}_cctv ${SOURCES})

target_link_libraries(${PROJECT_NAME}_cctv
    ${PROJECT_NAME}_util
    PkgConfig::FFMPEG
    ${PROJECT_NAME}_base
)

target_include_directories(${PROJECT_NAME}_cctv PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

install(TARGETS ${PROJECT_NAME}_cctv
        EXPORT ${PROJECT_NAME}_cctv-config
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${PROJECT_NAME}_cctv-config
        FILE ${PROJECT_NAME}_cctv-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}_cctv)
