include_directories(BEFORE SYSTEM ${gtest_SOURCE_DIR}/include)
include_directories(../include)

add_executable(masstransit_cpp_tests 
	src/message_mock.hpp
	src/message_consumer_mock.hpp
	
	src/main.cpp
	src/message_mock.cpp
	src/consume_context_tests.cpp
	src/bus_config_tests.cpp
)

target_link_libraries(masstransit_cpp_tests masstransit_cpp gtest)
add_test(masstransit_cpp_tests masstransit_cpp_tests)

install(TARGETS masstransit_cpp_tests
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)