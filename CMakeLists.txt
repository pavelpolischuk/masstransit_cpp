cmake_minimum_required(VERSION 3.6)

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
if(NOT BOOST_ROOT)
	set(BOOST_ROOT "c:/Projects/boost_1_61_0")
endif()

find_package(Boost REQUIRED COMPONENTS log date_time)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB -DBOOST_LOG_DYN_LINK")
set(AdditionalLibraries ${AdditionalLibraries} ${Boost_LIBRARIES})


if(NOT SimpleAmqpClient_DIR)
	set(SimpleAmqpClient_DIR "${CMAKE_CURRENT_SOURCE_DIR}/_install")
endif()

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(SimpleAmqpClient REQUIRED)
include_directories(SYSTEM ${SimpleAmqpClient_INCLUDE_DIRS})
set(AdditionalLibraries ${AdditionalLibraries} ${SimpleAmqpClient_LIBRARIES})

# Sources

set(SOURCE_FILES 
	include/masstransit_cpp/bus.hpp
	include/masstransit_cpp/bus_control.hpp
    include/masstransit_cpp/bus_factory.hpp
	include/masstransit_cpp/consume_context.hpp
	include/masstransit_cpp/global.hpp
	include/masstransit_cpp/host_info.hpp
	include/masstransit_cpp/i_message_consumer.hpp
	include/masstransit_cpp/i_publish_endpoint.hpp
	include/masstransit_cpp/json.hpp
	include/masstransit_cpp/json_adapters.hpp
	include/masstransit_cpp/message_consumer.hpp
	include/masstransit_cpp/message_handler.hpp
	include/masstransit_cpp/i_receive_endpoint.hpp
	include/masstransit_cpp/i_receive_endpoint_configurator.hpp

	include/masstransit_cpp/rabbit_mq/exchange_manager.hpp
	include/masstransit_cpp/rabbit_mq/rabbit_mq_bus.hpp
	include/masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint.hpp
	include/masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp
	include/masstransit_cpp/rabbit_mq/amqp_host.hpp
	include/masstransit_cpp/in_memory/in_memory_bus.hpp
	include/masstransit_cpp/in_memory/in_memory_configurator.hpp
	include/masstransit_cpp/in_memory/receive_endpoint.hpp
	include/masstransit_cpp/in_memory/receive_endpoint_configurator.hpp
	include/masstransit_cpp/threads/tasks_queue.hpp
	include/masstransit_cpp/threads/thread_pool.hpp
	include/masstransit_cpp/threads/worker.hpp
	include/masstransit_cpp/threads/worker_thread.hpp
	include/masstransit_cpp/threads/task_repeat.hpp
	
	src/bus_factory.cpp
	src/consume_context.cpp
	src/host_info.cpp
	src/json_adapters.cpp
	src/i_receive_endpoint.cpp
	src/i_receive_endpoint_configurator.cpp

	src/in_memory/receive_endpoint.cpp
	src/in_memory/receive_endpoint_configurator.cpp
	src/in_memory/in_memory_bus.cpp
	src/in_memory/in_memory_configurator.cpp
	src/rabbit_mq/amqp_host.cpp
	src/rabbit_mq/exchange_manager.cpp
	src/rabbit_mq/rabbit_mq_bus.cpp
	src/rabbit_mq/rabbit_mq_configurator.cpp
	src/rabbit_mq/receive_endpoint.cpp
	src/rabbit_mq/receive_endpoint_configurator.cpp
	src/threads/thread_pool.cpp
	src/threads/worker.cpp
	src/threads/worker_thread.cpp
	)

macro(OrganizeSources)
    foreach (_srcFile ${ARGN})
        get_filename_component(folder ${_srcFile} DIRECTORY)
        if ("${folder}_" STREQUAL "_")
            set(_NoSrcGroupFiles ${_NoSrcGroupFiles} ${_srcFile})
            continue()
        endif ("${folder}_" STREQUAL "_")
        string(REPLACE "/" "\\" _srcGroupName ${folder})
        set(_srcGroupFilesList "${_srcGroupName}_files")
        set(${_srcGroupName}_FILES ${${_srcGroupName}_FILES} ${_srcFile})
        list(LENGTH ${_srcGroupName}_FILES _currentGroupSize)
        if (${_currentGroupSize} EQUAL 1)
            set(_sourceGroups ${_sourceGroups} ${_srcGroupName})
        endif ()
    endforeach ()
    foreach (_srcGroup ${_sourceGroups})
        if (${CMAKE_VERBOSE_MAKEFILE})
            message("adding source group ${_srcGroup}")
            message("files in group:${${_srcGroup}_FILES}")
        endif (${CMAKE_VERBOSE_MAKEFILE})
        source_group(${_srcGroup} FILES ${${_srcGroup}_FILES})
    endforeach ()
    source_group("" FILES ${_NoSrcGroupFiles})
endmacro(OrganizeSources)

OrganizeSources(${SOURCE_FILES})

# Build

option(BUILD_SHARED_LIBS "Build MassTransit_Cpp as a shared library" OFF)
		
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMODULE_SETUP_DLL_EXPORTS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMASSTRANSIT_CPP_DLL_EXPORTS")
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


# Tests

option(BUILD_TESTS "Enable tests" OFF)

if (BUILD_TESTS)
    enable_testing()
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(masstransit_cpp_tests)
endif (BUILD_TESTS)


# Install

install(TARGETS masstransit_cpp
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
	
install(FILES
    include/masstransit_cpp/bus.hpp
    include/masstransit_cpp/bus_factory.hpp
	include/masstransit_cpp/consume_context.hpp
	include/masstransit_cpp/exchange_manager.hpp
	include/masstransit_cpp/global.hpp
	include/masstransit_cpp/host_info.hpp
	include/masstransit_cpp/i_bus_control.hpp
	include/masstransit_cpp/i_publish_endpoint.hpp
	include/masstransit_cpp/json.hpp
	include/masstransit_cpp/message_consumer.hpp
	include/masstransit_cpp/receive_endpoint.hpp
	include/masstransit_cpp/send_endpoint.hpp
	include/masstransit_cpp/uri.hpp
    DESTINATION include/masstransit_cpp
)