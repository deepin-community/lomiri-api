cmake_minimum_required(VERSION 2.8.10)

# Default install location. Must be set here, before setting the project.
if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "" FORCE)
endif()

project(lomiri-api C CXX)
set(PROJECT_VERSION "0.1.1")

if(${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
   message(FATAL_ERROR "In-tree build attempt detected, aborting. Set your build dir outside your source dir, delete CMakeCache.txt from source root and try again.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower) # Build types should always be lowercase but sometimes they are not.

include(PrecompiledHeaders)
find_package(CoverageReport)
#####################################################################
# Enable code coverage calculation with gcov/gcovr/lcov
# Usage:
#  * Switch build type to coverage (use ccmake or cmake-gui)
#  * Invoke make, make test, make coverage (or ninja if you use that backend)
#  * Find html report in subdir coveragereport
#  * Find xml report suitable for jenkins in coverage.xml
#####################################################################
if(cmake_build_type_lower MATCHES coverage)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage" )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage" )
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage" )
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage" )

  # We add -g when building with coverage so valgrind reports line numbers.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g" )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )

  # This allows us to skip the file descriptor closing test in Daemon_test
  # when coverage is enabled. (Closing file descriptors
  # in the test messes with coverage reporting.)
  add_definitions(-DCOVERAGE_ENABLED)
endif()

# Make sure we have all the needed symbols
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,defs")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,defs")

# Static C++ checks

find_program(CPPCHECK_COMMAND NAMES cppcheck)
if (CPPCHECK_COMMAND)
    set(CPPCHECK_COMMAND_OPTIONS --check-config --inline-suppr --enable=all -q --error-exitcode=2)
    set(CPPCHECK_COMMAND_OPTIONS ${CPPCHECK_COMMAND_OPTIONS} --template "{file}({line}): {severity} ({id}): {message}")
    add_custom_target(cppcheck COMMAND ${CPPCHECK_COMMAND} ${CPPCHECK_COMMAND_OPTIONS}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/test
        ${CMAKE_BINARY_DIR}/test
        VERBATIM
    )
else()
    message(WARNING "Cannot find cppcheck: cppcheck target will not be available")
endif()

#
# Definitions for testing with valgrind.
#

configure_file(CTestCustom.cmake.in CTestCustom.cmake) # Tests in CTestCustom.cmake are skipped for valgrind

find_program(MEMORYCHECK_COMMAND NAMES valgrind)
if (MEMORYCHECK_COMMAND)
    set(MEMORYCHECK_COMMAND_OPTIONS
        "--suppressions=${CMAKE_SOURCE_DIR}/valgrind-suppress --leak-check=full --num-callers=40 --error-exitcode=3"
    )
    add_custom_target(valgrind DEPENDS NightlyMemCheck)
else()
    message(WARNING "Cannot find valgrind: valgrind target will not be available")
endif()


include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)

# Standard install paths
include(GNUInstallDirs)

# Shell install paths
set(SHELL_PLUGINDIR ${CMAKE_INSTALL_LIBDIR}/lomiri/qml)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

# When building the library, we set the default symbol visibility
# to "hidden", so we don't export things by default.
# Exported functions and classes are prefixed by a LOMIRI_API macro,
# which explicitly exports a symbol if LOMIRI_DLL_EXPORTS is defined.
add_definitions(-DLOMIRI_DLL_EXPORTS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra")

# -fno-permissive causes warnings with clang, so we only enable it for gcc
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-permissive")

    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override" )
    endif()
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "relwithdebinfo")
    option(Werror "Treat warnings as errors" ON)
else()
    option(Werror "Treat warnings as errors" OFF)
endif()

if (Werror)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

# API version
set(LOMIRI_API_MAJOR 0)
set(LOMIRI_API_MINOR 1)
set(LOMIRI_API_MICRO 1)
set(LOMIRI_API_VERSION "${LOMIRI_API_MAJOR}.${LOMIRI_API_MINOR}.${LOMIRI_API_MICRO}")

# API library
set(LOMIRI_API_LIB lomiri-api)

# Static version for testing
set(LOMIRI_API_STATIC_LIB lomiri-api-static)

# Other libraries we depend on
set(OTHER_API_LIBS)

# All the libraries we need to link a normal executable that uses the Lomiri API
set(LIBS ${LOMIRI_API_LIB} ${OTHER_API_LIBS})

# All the libraries we need to link a gtest executable. (We link the tests against a static version
# so we can do whitebox testing on internal classes.
set(TESTLIBS ${LOMIRI_API_STATIC_LIB} ${OTHER_API_LIBS})

# Library install prefix
set(LIB_INSTALL_PREFIX lib/${CMAKE_LIBRARY_ARCHITECTURE})

# Tests
if (NOT NO_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
else()
    message(STATUS "Tests disabled")
endif()

# add subdirectories to build
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(data)

if (cmake_build_type_lower MATCHES coverage)
  ENABLE_COVERAGE_REPORT(TARGETS ${LOMIRI_API_LIB} FILTER /usr/include ${CMAKE_SOURCE_DIR}/test/* ${CMAKE_BINARY_DIR}/*)
endif()

#
# Documentation
#

find_package(Doxygen)
find_program(DOT_EXECUTABLE dot /usr/bin)
if (NOT DOXYGEN_FOUND OR NOT DOT_EXECUTABLE)
    message(WARNING "Cannot generate documentation: doxygen and/or graphviz not found")
else()
    configure_file(${PROJECT_SOURCE_DIR}/doc/Doxyfile.in ${PROJECT_BINARY_DIR}/doc/Doxyfile @ONLY IMMEDIATE)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doc/lib${LOMIRI_API_LIB}/index.html
                       COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doc/Doxyfile
                       DEPENDS ${PROJECT_BINARY_DIR}/doc/Doxyfile
                               ${LOMIRI_API_LIB_SRC}
                               ${LOMIRI_API_LIB_HDRS})
    add_custom_target(doc ALL
                       DEPENDS ${PROJECT_BINARY_DIR}/doc/lib${LOMIRI_API_LIB}/index.html)
    install(DIRECTORY ${PROJECT_BINARY_DIR}/doc/lib${LOMIRI_API_LIB}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc)
endif()
