add_subdirectory(api)
add_subdirectory(shell)
add_subdirectory(util)

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB internal_headers "${CMAKE_CURRENT_SOURCE_DIR}/internal/*.h")

install(FILES ${headers} DESTINATION ${HDR_INSTALL_DIR}/lomiri)

set(LOMIRI_API_LIB_HDRS ${LOMIRI_API_LIB_HDRS} ${headers} ${internal_headers} PARENT_SCOPE)
