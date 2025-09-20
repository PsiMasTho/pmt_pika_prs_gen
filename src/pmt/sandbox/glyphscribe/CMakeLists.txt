file(GLOB SOURCES "*.cpp")

add_executable(pmt_sandbox_wordlist_statistics_tui ${SOURCES})
target_link_libraries(pmt_sandbox_wordlist_statistics_tui pmt)

target_include_directories(pmt_sandbox_wordlist_statistics_tui PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)