set(LOMIRI_API_TEST_DATADIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(lomiri-api-test-config.h.in lomiri-api-test-config.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(gtest)
add_subdirectory(headers)
add_subdirectory(copyright)
add_subdirectory(whitespace)
add_subdirectory(qmltest)
