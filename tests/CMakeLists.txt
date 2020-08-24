include_directories(${BOOST_DI_DIR}/include)
include_directories(../include)


set(SOURCE_FILES
	include/catch.hpp
	include/error_handler_mock.hpp
	include/message_mock.hpp
	include/message_consumer_mock.hpp
	include/message_consumer_threads.hpp
	include/message_consumer_depend.hpp
	
	src/error_handler_mock.cpp
	src/main.cpp
	src/message_mock.cpp
	src/message_consumer_mock.cpp
	src/message_consumer_threads.cpp
	src/message_consumer_depend.cpp
	
	src/tests/amqp_ack_tests.cpp
	src/tests/amqp_channels_tests.cpp
	src/tests/amqp_connect_tests.cpp
	src/tests/amqp_consume_tests.cpp
	src/tests/amqp_exchange_tests.cpp
	src/tests/amqp_get_tests.cpp
	src/tests/amqp_host_tests.cpp
	src/tests/amqp_message_tests.cpp
	src/tests/amqp_nack_tests.cpp
	src/tests/amqp_publish_tests.cpp
	src/tests/amqp_queue_tests.cpp
	src/tests/amqp_table_tests.cpp
	src/tests/consume_context_tests.cpp
	src/tests/in_memory_bus_config_tests.cpp
	src/tests/bus_config_tests.cpp
	src/tests/bus_di_config_tests.cpp
	src/tests/worker_tests.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/include")
add_executable(testing ${SOURCE_FILES})
target_link_libraries(testing masstransit_cpp)

add_test(NAME testing COMMAND testing WORKING_DIRECTORY bin)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES})

install(TARGETS testing
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
