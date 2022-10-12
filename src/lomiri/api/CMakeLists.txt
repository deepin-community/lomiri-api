add_subdirectory(internal)

set(API_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp
)

set(LOMIRI_API_LIB_SRC ${LOMIRI_API_LIB_SRC} ${API_SRC} PARENT_SCOPE)
