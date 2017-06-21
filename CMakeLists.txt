cmake_minimum_required(VERSION 3.6)
project(masstransit_cpp)

include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

option(MasstransitCppBuildSharedLIB "MasstransitCpp is shared" OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(NOT DEFINED BOOST_ROOT)
	set(BOOST_ROOT C:/Projects/boost_1_61_0)
endif(NOT DEFINED BOOST_ROOT)
message("BOOST_ROOT=${BOOST_ROOT}")

set(Googletest_DIR ${CMAKE_SOURCE_DIR}/3rd_party/googletest_binary)
set(SimpleAmqpClient_DIR ${CMAKE_SOURCE_DIR}/3rd_party/simple_amqp_client_binary)


################ Boost ######################
find_package(Boost REQUIRED COMPONENTS log)
CheckDependency(Boost)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB -DBOOST_LOG_DYN_LINK")
set(AdditionalLibraries ${AdditionalLibraries} ${Boost_LIBRARIES})

################ SimpleAmqpClient ###########
find_package(SimpleAmqpClient)
CheckDependency(SimpleAmqpClient)
include_directories(${SimpleAmqpClient_INCLUDE_DIRS})
set(AdditionalLibraries ${AdditionalLibraries} ${SimpleAmqpClient_LIBRARY})

################ Sources ####################
set(SOURCE_FILES 
	include/masstransit_cpp/bus.hpp
	include/masstransit_cpp/consume_context.hpp
	include/masstransit_cpp/exchange_manager.hpp
	include/masstransit_cpp/global.hpp
	include/masstransit_cpp/host_info.hpp
	include/masstransit_cpp/i_bus.hpp
	include/masstransit_cpp/json.hpp
	include/masstransit_cpp/message_consumer.hpp
	include/masstransit_cpp/receive_endpoint.hpp
	include/masstransit_cpp/send_endpoint.hpp
	include/masstransit_cpp/uri.hpp
	
	src/bus.cpp
	src/consume_context.cpp
	src/exchange_manager.cpp
	src/host_info.cpp
	src/receive_endpoint.cpp
	src/send_endpoint.cpp
	src/uri.cpp
)
		
if(${MasstransitCppBuildSharedLIB})
set(LINK_TYPE SHARED)
else()
set(LINK_TYPE STATIC)
endif()

add_library(masstransit_cpp ${LINK_TYPE} ${SOURCE_FILES})

target_link_libraries(masstransit_cpp ${AdditionalLibraries})

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(masstransit_cpp_tests)