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

find_package(Boost REQUIRED COMPONENTS log date_time)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB -DBOOST_LOG_DYN_LINK")
set(AdditionalLibraries ${AdditionalLibraries} ${Boost_LIBRARIES})


SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(SimpleAmqpClient REQUIRED)
include_directories(SYSTEM ${SimpleAmqpClient_INCLUDE_DIRS})
set(AdditionalLibraries ${AdditionalLibraries} ${SimpleAmqpClient_LIBRARIES})

# Sources

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


# Build

option(BUILD_SHARED_LIBS "Build MassTransit_Cpp as a shared library" OFF)
		
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMODULE_SETUP_DLL_EXPORTS")
if(BUILD_SHARED_LIBS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONTROLS_DLL")
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
    set(BUILD_GTEST ON)
    set(BUILD_SHARED_LIBS OFF)
    set(gtest_force_shared_crt ON)
	
    mark_as_advanced(BUILD_GTEST)
    mark_as_advanced(gmock_build_tests)
    mark_as_advanced(gtest_build_samples)
    mark_as_advanced(gtest_build_tests)
    mark_as_advanced(gtest_disable_pthreads)
    mark_as_advanced(gtest_force_shared_crt)
    mark_as_advanced(gtest_hide_internal_symbols)
    add_subdirectory(3rd_party/googletest)
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
    DESTINATION include/masstransit_cpp
)