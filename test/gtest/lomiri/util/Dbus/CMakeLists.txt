add_executable(Dbus_test Dbus_test.cpp)
target_link_libraries(Dbus_test ${LIBS} ${TESTLIBS})

add_test(Dbus Dbus_test)
