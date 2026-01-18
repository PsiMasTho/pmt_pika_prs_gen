cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pika_parser_generator VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

add_subdirectory(src)

install(DIRECTORY skel/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/skel
        COMPONENT skel)

add_custom_target(install-skel
 COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --component skel
 COMMENT "Installing skeleton files")
