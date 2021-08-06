#
# Author: Tassilo Tanneberger on 2/8/2021.
# Owner: GFaI e.V
# Project: Robo Simulator
#
cmake_minimum_required (VERSION 3.8)
project (robo-emulator)

# Checking GNUDirs and the folders on the build server
include(GNUInstallDirs)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# C++17
set(CMAKE_CXX_STANDARD 17)


# build options
set(GFAI_BUILD ON CACHE BOOL "Boolean flag for using the build server of the GFAI")
set (GFAI_BUILD ON) #TODO: REMOVE

# gfai build server and tool chain
if(${GFAI_BUILD})
	message("USING THE INTERNAL GFAI BUILD SERVER")
	include(cmake/gfai.cmake)
endif()

# enable/disable unit test
option(BUILD_TESTS "Build test programs" OFF)

find_package(spdlog CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

add_library(RoboSimulator_LIB STATIC 
		src/network/socket.hpp
		src/network/socket.cpp
		src/network/public_server.hpp
		src/network/public_server.cpp
		src/network/client.hpp
		src/network/client.cpp
		src/network/udp_socket.hpp
		src/network/udp_socket.cpp
		src/recorder/record_entry.hpp
		src/recorder/record_entry.cpp
		src/recorder/recorder.hpp
		src/recorder/recorder.cpp
		src/signal_handler.hpp
		src/recorder/record_server.hpp
		src/recorder/record_server.cpp
		src/network/crc_hash.hpp 
		src/type_traits/type_traits.hpp  
		src/environment/environment.hpp
		src/environment/environment.cpp
		)


# Add source to this project's executable.
#add_executable (RoboSimulator main.cpp)
add_executable (Client src/client_main.cpp)
add_executable (Server src/server_main.cpp)

# windows libraries
if (WIN32)
	target_link_libraries(RoboSimulator_LIB PRIVATE wsock32 ws2_32)
endif(WIN32)

# unix libs
if(UNIX)
    target_link_libraries(Server stdc++fs pthread)
    target_link_libraries(Client stdc++fs pthread)
endif(UNIX)

# add library heads to include path
include_directories("${SPDLOG_INCLUDE_DIR}")
include_directories("${JSONCPP_INCLUDE_DIR}")
# link external libs
target_link_libraries(RoboSimulator_LIB PRIVATE spdlog::spdlog JsonCpp::JsonCpp )
target_link_libraries(Client RoboSimulator_LIB spdlog::spdlog)
target_link_libraries(Server RoboSimulator_LIB spdlog::spdlog)

if(MSVC)
     #add_definitions(/W3 /WX)
	# add_definitions(/MD)
else()
     add_definitions(-Wall -Wextra -pedantric -Werror)
endif()



# Unit test of the enviromnent 
if(${BUILD_TESTS})
    enable_testing()
    find_package(GTest 1.10.0 EXACT REQUIRED)

    # Change the internal variable to google aad test script.
    # Use our own version of GoogleTestAddTests.cmake
    set(_GOOGLETEST_DISCOVER_TESTS_SCRIPT
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GoogleTestAddTests.cmake
    )
endif()
