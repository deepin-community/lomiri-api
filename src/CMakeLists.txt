add_subdirectory(lomiri)

include_directories(${GLIB_INCLUDE_DIRS})

# Pseudo-library of object files. We need a dynamic version of the library for normal clients,
# and a static version for the whitebox tests, so we can write unit tests for classes in the internal namespaces
# (because, for the .so, non-public APIs are compiled with -fvisibility=hidden).
# Everything is compiled with -fPIC, so the same object files are suitable for either library.
# Here, we create an object library that then is used to link the static and dynamic
# libraries, without having to compile each source file twice with different compile flags.

set(LOMIRI_API_LIB_OBJ ${LOMIRI_API_LIB}-obj)
add_library(${LOMIRI_API_LIB_OBJ} OBJECT ${LOMIRI_API_LIB_SRC})
set_target_properties(${LOMIRI_API_LIB_OBJ} PROPERTIES COMPILE_FLAGS "-fPIC")
add_pch(pch/lomiriapi_pch.hh ${LOMIRI_API_LIB_OBJ})

# Use the object files to make the shared library.
set(LOMIRI_API_SOVERSION 0)
add_library(${LOMIRI_API_LIB} SHARED $<TARGET_OBJECTS:${LOMIRI_API_LIB_OBJ}>)
set_target_properties(${LOMIRI_API_LIB} PROPERTIES
    VERSION "${LOMIRI_API_MAJOR}.${LOMIRI_API_MINOR}"
    SOVERSION ${LOMIRI_API_SOVERSION}
)
target_link_libraries(${LOMIRI_API_LIB} ${GLIB_LDFLAGS})

# Use the object files to make the static library. We add -fPIC to avoid compiling a second time.
add_library(${LOMIRI_API_STATIC_LIB} STATIC $<TARGET_OBJECTS:${LOMIRI_API_LIB_OBJ}>)
set_target_properties(${LOMIRI_API_STATIC_LIB} PROPERTIES OUTPUT_NAME ${LOMIRI_API_LIB})
target_link_libraries(${LOMIRI_API_STATIC_LIB} ${GLIB_LDFLAGS})

# Only the dynamic library gets installed.
install(TARGETS ${LOMIRI_API_LIB} LIBRARY DESTINATION ${LIB_INSTALL_PREFIX})

# Parent needs to know what all the source files are, for generating doc and the like.
set(LOMIRI_API_LIB_SRC ${LOMIRI_API_LIB_SRC} ${LOMIRI_SRC} PARENT_SCOPE)
