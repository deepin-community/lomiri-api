add_subdirectory(api)
add_subdirectory(internal)
add_subdirectory(util)

set(LOMIRI_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LomiriExceptions.cpp
)

set(LOMIRI_API_LIB_SRC ${LOMIRI_API_LIB_SRC} ${LOMIRI_SRC} PARENT_SCOPE)
