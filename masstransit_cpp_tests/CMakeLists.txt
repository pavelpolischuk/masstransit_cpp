cmake_minimum_required(VERSION 3.6)
project(masstransit_cpp_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMODULE_SETUP_DLL_EXPORTS")

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 /MP")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd\"4005\" /wd\"4251\" /wd\"4275\" /wd\"4706\" -D_SCL_SECURE_NO_WARNINGS")
else()
	message("Not ready for non-msvc compilation")
endif ()

if(BUILD_SHARED_LIBS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONTROLS_DLL")
endif()

set(AdditionalLibraries masstransit_cpp)

################ Boost ######################
find_package(Boost REQUIRED COMPONENTS system log)
CheckDependency(Boost)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB -DBOOST_LOG_DYN_LINK")
set(AdditionalLibraries ${AdditionalLibraries} ${Boost_LIBRARIES})

################ GOOGLETEST #################
find_package(Googletest)
CheckDependency(Googletest)
include_directories(${Googletest_INCLUDE_DIRS})
set(AdditionalLibraries ${AdditionalLibraries} ${Googletest_LIBRARY})
set(AdditionalLibraries ${AdditionalLibraries} ${Googlemock_LIBRARY})

################ SimpleAmqpClient ###########
find_package(SimpleAmqpClient)
CheckDependency(SimpleAmqpClient)
include_directories(${SimpleAmqpClient_INCLUDE_DIRS})
set(AdditionalLibraries ${AdditionalLibraries} ${SimpleAmqpClient_LIBRARY})

################ Sources ####################
set(SOURCE_FILES 
	src/main.cpp
	src/message_mock.hpp
	src/message_mock.cpp
	src/consume_context_tests.cpp
	src/message_consumer_mock.hpp
	src/bus_config_tests.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${masstransit_cpp_SOURCE_DIR}/include")

add_executable(masstransit_cpp_tests ${SOURCE_FILES})
target_link_libraries(masstransit_cpp_tests ${AdditionalLibraries})

CopyDependencies(TARGET masstransit_cpp_tests LIBRARIES ${AdditionalLibraries} ${SimpleAmqpClient_DEPS})