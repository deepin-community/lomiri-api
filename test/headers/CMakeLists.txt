#
# Test that all header files compile stand-alone and that no public header includes an internal one.
#

set(root_inc_dir ${CMAKE_SOURCE_DIR}/include)

set(subdirs
    lomiri
    lomiri/api
    lomiri/util
)

set(exclusions
    "GioMemory.h"
    "GlibMemory.h"
    "GObjectMemory.h"
)

foreach(dir ${subdirs})

    string(REPLACE "/" "-" location ${dir})

    set(public_inc_dir ${root_inc_dir}/${dir})
    set(internal_inc_dir ${public_inc_dir}/internal)

    # Test that each public header compiles stand-alone.
    add_test(stand-alone-${location}-headers
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_headers.py
            ${public_inc_dir} ${CMAKE_CXX_COMPILER} "-I${root_inc_dir} -I${public_inc_dir} ${CMAKE_CXX_FLAGS}" "--exclusions" ${exclusions})

    # Test that each internal header compiles stand-alone,
    # if internal headers exist.
    if(EXISTS ${internal_inc_dir})
      add_test(stand-alone-${location}-internal-headers
          ${CMAKE_CURRENT_SOURCE_DIR}/compile_headers.py
              ${internal_inc_dir} ${CMAKE_CXX_COMPILER} "-I${root_inc_dir} -I${internal_inc_dir} ${CMAKE_CXX_FLAGS}" "--exclusions" ${exclusions})
    endif(EXISTS ${internal_inc_dir})

    # Test that no public header includes an internal header
    add_test(clean-public-${location}-headers ${CMAKE_CURRENT_SOURCE_DIR}/check_public_headers.py ${public_inc_dir})

endforeach(dir)

add_test(cleanincludes ${CMAKE_CURRENT_SOURCE_DIR}/includechecker.py ${CMAKE_SOURCE_DIR}/include)
