find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
set(TESTLIBS ${TESTLIBS} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# gtest does weird things with its own implementation of tr1::tuple. For clang, we need to
# set this macro, otherwise anything that includes gtest.h won't compile.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_USE_OWN_TR1_TUPLE=1")
endif()

add_subdirectory(lomiri)

set(TEST_SRC
)

foreach(src ${TEST_SRC})
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
    target_link_libraries(${name} ${TESTLIBS})
    string(REPLACE "_test" "" test_name ${name})
    add_test(${test_name} ${name})
endforeach(src)
