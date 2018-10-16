include_directories(${BOOST_DI_DIR}/include)
include_directories(../include)

set(CXX_STANDARD "c++14")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pedantic-errors -Wall -Wextra")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pedantic-errors -Wall -Wextra")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc")
else()
    message(WARNING "Unsupported compiler!")
endif()

add_executable(masstransit_tests 
	src/catch.hpp
	src/message_mock.hpp
	src/message_consumer_mock.hpp
	src/message_consumer_depend.hpp
	
	src/main.cpp
	src/message_mock.cpp
	src/message_consumer_depend.cpp
	
	src/consume_context_tests.cpp
	src/in_memory_bus_config_tests.cpp
	src/bus_config_tests.cpp
	src/bus_di_config_tests.cpp
	src/amqp_host_tests.cpp
	src/worker_tests.cpp
)

target_link_libraries(masstransit_tests masstransit_cpp)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES})

install(TARGETS masstransit_tests
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
