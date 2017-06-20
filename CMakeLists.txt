cmake_minimum_required(VERSION 3.6)
project(masstransit_cpp)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(NOT DEFINED BOOST_ROOT)
	set(BOOST_ROOT C:/Projects/boost_1_61_0)
endif(NOT DEFINED BOOST_ROOT)
message("BOOST_ROOT=${BOOST_ROOT}")

set(Googletest_DIR ${CMAKE_SOURCE_DIR}/3rd_party/googletest_binary)
set(SimpleAmqpClient_DIR ${CMAKE_SOURCE_DIR}/3rd_party/simple_amqp_client_binary)

add_subdirectory(masstransit_cpp)

