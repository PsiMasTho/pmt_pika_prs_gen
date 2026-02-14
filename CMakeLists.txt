cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pmt_pika VERSION 1.0.0 LANGUAGES CXX)

find_package(pmt)

include(GNUInstallDirs)

add_subdirectory(src)

add_subdirectory(examples EXCLUDE_FROM_ALL)

install(DIRECTORY skel/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/skel
        COMPONENT skel)

add_custom_target(install-skel
 COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --component skel
 COMMENT "Installing skeleton files")
