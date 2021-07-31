# CMakeList.txt : CMake project for RoboSimulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project ("robo-emulator")
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
#set(DCMAKE_TOOLCHAIN_FILE "C:/Users/EinSpaten/Documents/vcpkg/scripts/buildsystems/vcpkg.cmake")

#include_directories("C:/Program Files (x86)/spdlog/include/")
#set(spdlog_DIR "C:/Program Files (x86)/spdlog/lib/cmake") # spdlog installed directory ("lib/spdlog/spdlogConfig.cmake" exists in this direcotry).
find_package(spdlog CONFIG REQUIRED)
#target_link_libraries(main PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
find_package(jsoncpp CONFIG REQUIRED)



add_library(RoboSimulator_LIB STATIC 
		src/network/socket.hpp
		src/network/socket.cpp
		src/network/public_server.hpp
		src/network/public_server.cpp
		src/network/client.hpp
		src/network/client.cpp
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



if (WIN32)
target_link_libraries(RoboSimulator_LIB wsock32 ws2_32)
endif(WIN32)

if(UNIX)
    target_link_libraries(Server stdc++fs pthread)
    target_link_libraries(Client stdc++fs pthread)
endif(UNIX)


target_link_libraries(RoboSimulator_LIB spdlog::spdlog jsoncpp_lib jsoncpp_object)

target_link_libraries(Client RoboSimulator_LIB spdlog::spdlog)
target_link_libraries(Server RoboSimulator_LIB spdlog::spdlog)

if(MSVC)
     #add_definitions(/W3 /WX)
	 add_definitions(/MD)
else()
     add_definitions(-Wall -Wextra -pedantric -Werror)
endif()





# TODO: Add tests and install targets if needed.
