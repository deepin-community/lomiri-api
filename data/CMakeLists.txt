# Set up package config.
set(PKGCONFIG_NAME "lomiri-shell-api")
set(PKGCONFIG_DESCRIPTION "Lomiri shell APIs")
set(PKGCONFIG_PLUGINDIR "${SHELL_PLUGINDIR}")
set(VERSION "1.0")

configure_file(lomiri-shell-api.pc.in lomiri-shell-api.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lomiri-shell-api.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

configure_file(lib${LOMIRI_API_LIB}.pc.in lib${LOMIRI_API_LIB}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${LOMIRI_API_LIB}.pc DESTINATION ${LIB_INSTALL_PREFIX}/pkgconfig)
