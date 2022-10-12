set(INCLUDE_INSTALL_DIR include/lomiri/shell/application)

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB internal_headers "${CMAKE_CURRENT_SOURCE_DIR}/internal/*.h")

add_custom_target(appheaders
                  SOURCES ${headers} ${internal_headers})

install(FILES ${headers} DESTINATION ${INCLUDE_INSTALL_DIR})

set(LOMIRI_API_LIB_HDRS ${LOMIRI_API_LIB_HDRS} ${headers} ${internal_headers} PARENT_SCOPE)

set(VERSION 27)
set(PKGCONFIG_NAME "lomiri-shell-application")
set(PKGCONFIG_DESCRIPTION "Lomiri shell Application APIs")
set(PKGCONFIG_REQUIRES "Qt5Core")
set(PKGCONFIG_FILE lomiri-shell-application.pc)

configure_file(${CMAKE_SOURCE_DIR}/data/lomiri-shell-api.pc.in
  ${CMAKE_BINARY_DIR}/data/${PKGCONFIG_FILE} @ONLY
)

install(FILES ${CMAKE_BINARY_DIR}/data/${PKGCONFIG_FILE}
  DESTINATION ${LIB_INSTALL_PREFIX}/pkgconfig
)
