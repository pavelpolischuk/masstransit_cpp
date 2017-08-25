include_directories(../include)

add_executable(masstransit_cpp_tests 
	src/catch.hpp
	src/message_mock.hpp
	src/message_consumer_mock.hpp
	
	src/main.cpp
	src/message_mock.cpp
	src/consume_context_tests.cpp
	src/bus_config_tests.cpp
	src/amqp_host_tests.cpp
	src/worker_tests.cpp
)

target_link_libraries(masstransit_cpp_tests masstransit_cpp)
add_test(masstransit_cpp_tests masstransit_cpp_tests)

install(TARGETS masstransit_cpp_tests
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)