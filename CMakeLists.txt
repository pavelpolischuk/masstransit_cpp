cmake_minimum_required(VERSION 3.10)

project(masstransit_cpp)


# Versions

file(STRINGS include/masstransit_cpp/global.hpp _VERSION_MAJOR REGEX "^#define MASSTRANSIT_CPP_VERSION_MAJOR [0-9]+$")
file(STRINGS include/masstransit_cpp/global.hpp _VERSION_MINOR REGEX "^#define MASSTRANSIT_CPP_VERSION_MINOR [0-9]+$")
file(STRINGS include/masstransit_cpp/global.hpp _VERSION_PATCH REGEX "^#define MASSTRANSIT_CPP_VERSION_PATCH [0-9]+$")

string(REGEX MATCH "[0-9]+" _VERSION_MAJOR ${_VERSION_MAJOR})
string(REGEX MATCH "[0-9]+" _VERSION_MINOR ${_VERSION_MINOR})
string(REGEX MATCH "[0-9]+" _VERSION_PATCH ${_VERSION_PATCH})

set(MTC_VERSION ${_VERSION_MAJOR}.${_VERSION_MINOR}.${_VERSION_PATCH})
set(MTC_SOVERSION ${_VERSION_MAJOR})


# Dependencies

find_package(Boost)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB")

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Rabbitmqc REQUIRED)
include_directories(SYSTEM ${Rabbitmqc_INCLUDE_DIRS})
set(AdditionalLibraries ${AdditionalLibraries} ${Rabbitmqc_LIBRARIES})


# Sources

set(SOURCE_FILES 
	include/masstransit_cpp/bus.hpp
	include/masstransit_cpp/bus_control.hpp
	include/masstransit_cpp/bus_factory.hpp
	include/masstransit_cpp/consume_context.hpp
	include/masstransit_cpp/consume_context_info.hpp
	include/masstransit_cpp/global.hpp
	include/masstransit_cpp/host_info.hpp
	include/masstransit_cpp/i_message_consumer.hpp
	include/masstransit_cpp/i_publish_endpoint.hpp
	include/masstransit_cpp/message_consumer.hpp
	include/masstransit_cpp/message_handler.hpp
	include/masstransit_cpp/i_receive_endpoint.hpp
	include/masstransit_cpp/receive_endpoint_config_base.hpp
	include/masstransit_cpp/i_receive_endpoint_configurator.hpp
	
	include/masstransit_cpp/in_memory/in_memory_bus.hpp
	include/masstransit_cpp/in_memory/in_memory_configurator.hpp
	include/masstransit_cpp/in_memory/receive_endpoint.hpp
	include/masstransit_cpp/in_memory/receive_endpoint_configurator.hpp

	include/masstransit_cpp/rabbit_mq/amqp_channel.hpp
	include/masstransit_cpp/rabbit_mq/amqp_envelope.hpp
	include/masstransit_cpp/rabbit_mq/amqp_host.hpp
	include/masstransit_cpp/rabbit_mq/amqp_host_configurator.hpp
	include/masstransit_cpp/rabbit_mq/amqp_message.hpp
	include/masstransit_cpp/rabbit_mq/amqp_table.hpp
	include/masstransit_cpp/rabbit_mq/exceptions.hpp
	include/masstransit_cpp/rabbit_mq/exchange_manager.hpp
	include/masstransit_cpp/rabbit_mq/message_publisher.hpp
	include/masstransit_cpp/rabbit_mq/rabbit_mq_bus.hpp
	include/masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint_config.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp
	include/masstransit_cpp/rabbit_mq/ssl_config.hpp

	include/masstransit_cpp/threads/queue_worker.hpp
	include/masstransit_cpp/threads/task_repeat.hpp
	include/masstransit_cpp/threads/tasks_queue.hpp
	include/masstransit_cpp/threads/thread_pool.hpp
	include/masstransit_cpp/threads/worker.hpp
	include/masstransit_cpp/threads/worker_pool.hpp
	include/masstransit_cpp/threads/worker_thread.hpp

	include/masstransit_cpp/utils/console_error_handler.hpp
	include/masstransit_cpp/utils/datetime.hpp
	include/masstransit_cpp/utils/i_error_handler.hpp
	include/masstransit_cpp/utils/json.hpp
	include/masstransit_cpp/utils/json_adapters.hpp
	

	src/bus_factory.cpp
	src/consume_context_info.cpp
	src/host_info.cpp
	src/i_receive_endpoint.cpp
	src/i_receive_endpoint_configurator.cpp

	src/in_memory/receive_endpoint.cpp
	src/in_memory/receive_endpoint_configurator.cpp
	src/in_memory/in_memory_bus.cpp
	src/in_memory/in_memory_configurator.cpp

	src/rabbit_mq/amqp_channel.cpp
	src/rabbit_mq/amqp_envelope.cpp
	src/rabbit_mq/amqp_host.cpp
	src/rabbit_mq/amqp_host_configurator.cpp
	src/rabbit_mq/amqp_message.cpp
	src/rabbit_mq/amqp_table.cpp
	src/rabbit_mq/exceptions.cpp
	src/rabbit_mq/exchange_manager.cpp
	src/rabbit_mq/message_publisher.cpp
	src/rabbit_mq/rabbit_mq_bus.cpp
	src/rabbit_mq/rabbit_mq_configurator.cpp
	src/rabbit_mq/receive_endpoint.cpp
	src/rabbit_mq/receive_endpoint_config.cpp
	src/rabbit_mq/receive_endpoint_configurator.cpp
	src/rabbit_mq/ssl_config.cpp

	src/threads/queue_worker.cpp
	src/threads/task_repeat.cpp
	src/threads/tasks_queue.cpp
	src/threads/thread_pool.cpp
	src/threads/worker.cpp
	src/threads/worker_thread.cpp
	src/threads/worker_pool.cpp

	src/utils/datetime.cpp
	src/utils/errors_handlers.cpp
	src/utils/json_adapters.cpp
)


# Build

option(BUILD_SHARED_LIBS "Build MassTransit_Cpp as a shared library" OFF)
		
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMODULE_SETUP_DLL_EXPORTS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMASSTRANSIT_CPP_DLL_EXPORTS")
if (WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 /wd4005 /wd4503 /wd4290")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pedantic-errors -Wall -Wextra -pthread")
endif ()

if(BUILD_SHARED_LIBS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONTROLS_DLL -DMASSTRANSIT_CPP_DLL")
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")
add_library(masstransit_cpp ${SOURCE_FILES})
target_link_libraries(masstransit_cpp ${AdditionalLibraries})

if (WIN32)
	set_target_properties(masstransit_cpp PROPERTIES VERSION ${MTC_VERSION} OUTPUT_NAME masstransit_cpp.${MTC_SOVERSION})
else ()
	set_target_properties(masstransit_cpp PROPERTIES VERSION ${MTC_VERSION} SOVERSION ${MTC_SOVERSION})
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS 1)
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Tests

option(BUILD_TESTS "Enable tests" OFF)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif (BUILD_TESTS)


# Install

install(TARGETS masstransit_cpp
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
	
install(FILES
	include/masstransit_cpp/bus.hpp
	include/masstransit_cpp/bus_control.hpp
	include/masstransit_cpp/bus_factory.hpp
	include/masstransit_cpp/consume_context.hpp
	include/masstransit_cpp/consume_context_info.hpp
	include/masstransit_cpp/global.hpp
	include/masstransit_cpp/host_info.hpp
	include/masstransit_cpp/i_message_consumer.hpp
	include/masstransit_cpp/i_publish_endpoint.hpp
	include/masstransit_cpp/i_receive_endpoint.hpp
	include/masstransit_cpp/i_receive_endpoint_configurator.hpp
	include/masstransit_cpp/message_consumer.hpp
	include/masstransit_cpp/message_handler.hpp
	include/masstransit_cpp/receive_endpoint_config_base.hpp
	DESTINATION include/masstransit_cpp
)

install(FILES
	include/masstransit_cpp/rabbit_mq/amqp_host.hpp
	include/masstransit_cpp/rabbit_mq/amqp_host_configurator.hpp
	include/masstransit_cpp/rabbit_mq/exchange_manager.hpp
	include/masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint_config.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp
	include/masstransit_cpp/rabbit_mq/ssl_config.hpp
	DESTINATION include/masstransit_cpp/rabbit_mq
)

install(FILES
	include/masstransit_cpp/in_memory/in_memory_configurator.hpp
	include/masstransit_cpp/in_memory/receive_endpoint.hpp
	include/masstransit_cpp/in_memory/receive_endpoint_configurator.hpp
	DESTINATION include/masstransit_cpp/in_memory
)

install(FILES
	include/masstransit_cpp/threads/queue_worker.hpp
	include/masstransit_cpp/threads/task_repeat.hpp
	include/masstransit_cpp/threads/tasks_queue.hpp
	include/masstransit_cpp/threads/worker.hpp
	include/masstransit_cpp/threads/worker_pool.hpp
	include/masstransit_cpp/threads/worker_thread.hpp
	DESTINATION include/masstransit_cpp/threads
)

install(FILES
	include/masstransit_cpp/utils/console_error_handler.hpp
	include/masstransit_cpp/utils/i_error_handler.hpp
	include/masstransit_cpp/utils/json.hpp
	include/masstransit_cpp/utils/json_adapters.hpp
  	DESTINATION include/masstransit_cpp/utils
)  
