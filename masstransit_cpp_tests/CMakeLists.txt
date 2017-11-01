include_directories(../include)
include_directories(${BOOST_DI_DIR}/include)

set(CXX_STANDARD "c++14")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -fno-exceptions -pedantic -pedantic-errors -Wall -Wextra -Werror")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -fno-exceptions -pedantic -pedantic-errors -Wall -Wextra -Werror")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc")
else()
    message(WARNING "Unsupported compiler!")
endif()

add_executable(masstransit_cpp_tests 
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

target_link_libraries(masstransit_cpp_tests masstransit_cpp)
add_test(masstransit_cpp_tests masstransit_cpp_tests)

install(TARGETS masstransit_cpp_tests
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)