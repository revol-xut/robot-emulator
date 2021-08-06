#
# Author: Tassilo Tanneberger on 2/8/2021.
# Owner: GFaI e.V
# Project: Robo Simulator
#

set(IS_DOWNLOAD_LIBS OFF CACHE BOOL "Boolean flag to download libraries to build folder")
set(ROBO_SIM_LIBRARY_PATH "\\\\BVIA-BUILD//Robo-Simulator/" CACHE  PATH "Path to external libraries")
set(ROBO_SIM_LIBRARY_PATH "\\\\BVIA-BUILD/KNAUF") #TODO: REMOVE

message(STATUS "Build path: ${ROBO_SIM_LIBRARY_PATH}")

if (${IS_DOWNLOAD_LIBS})
	set(BVIA_BUILD_LIBS_PATH "${CMAKE_BINARY_DIR}" CACHE PATH "Path to built/install external libraries." FORCE)
else()
	set(BVIA_BUILD_LIBS_PATH "${ROBO_SIM_LIBRARY_PATH}" CACHE PATH "Path to built/install external libraries." FORCE)
endif()


list(APPEND CMAKE_PREFIX_PATH	"${BVIA_BUILD_LIBS_PATH}\\googletest-1.10.0\\install"
								"${BVIA_BUILD_LIBS_PATH}\\spdlog-1.8.5\\install"
 								"${BVIA_BUILD_LIBS_PATH}\\jsoncpp_1.9.4\\install" )


if (NOT EXISTS "${CMAKE_BINARY_DIR}/googletest-1.10.0/install")
    message(STATUS "Copying googletest ...")
    file(COPY "${ROBO_SIM_LIBRARY_PATH}/googletest-1.10.0/install" DESTINATION "${CMAKE_BINARY_DIR}/googletest-1.10.0")
endif()
if (NOT EXISTS "${CMAKE_BINARY_DIR}/spdlog-1.8.5/install")
    message(STATUS "Copying spdlog ...")
    file(COPY "${ROBO_SIM_LIBRARY_PATH}/spdlog-1.8.5/install" DESTINATION "${CMAKE_BINARY_DIR}/spdlog-1.8.5")
endif()
if (NOT EXISTS "${CMAKE_BINARY_DIR}/jsoncpp_1.9.4/install")
    message(STATUS "Copying jsoncpp ...")
    file(COPY "${ROBO_SIM_LIBRARY_PATH}/jsoncpp_1.9.4/install" DESTINATION "${CMAKE_BINARY_DIR}/jsoncpp_1.9.4")
endif()

set(SPDLOG_INCLUDE_DIR "${CMAKE_BINARY_DIR}/spdlog-1.8.5/install/include/")
set(JSONCPP_INCLUDE_DIR "${CMAKE_BINARY_DIR}/jsoncpp_1.9.4/install/include/")

function (unsetVariablesStartingWith _prefix)
    get_cmake_property(_vars CACHE_VARIABLES)
    string(REGEX MATCHALL "(^|;)${_prefix}[A-Za-z0-9_-]*" _matchedVars "${_vars}")
    foreach (_variable ${_matchedVars})
        get_property(_type CACHE "${_variable}" PROPERTY TYPE)
        if (NOT "${_type}" STREQUAL "STATIC") 
            unset("${_variable}" CACHE)
        endif()
    endforeach()
endfunction()

list(APPEND prefixes spdlog GTest jsoncpp)
foreach (prefix ${prefixes})
    unsetVariablesStartingWith(${prefix})
endforeach()

