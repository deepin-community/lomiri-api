file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB internal_headers "${CMAKE_CURRENT_SOURCE_DIR}/internal/*.h")

#
# Generated headers
#
configure_file(Version.h.in Version.h)
set(headers ${headers} ${CMAKE_CURRENT_BINARY_DIR}/Version.h)

install(FILES ${headers} DESTINATION ${HDR_INSTALL_DIR}/lomiri/api)

set(LOMIRI_API_LIB_HDRS ${LOMIRI_API_LIB_HDRS} ${headers} ${internal_headers} PARENT_SCOPE)
