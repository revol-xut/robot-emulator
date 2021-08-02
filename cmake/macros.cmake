# fill variables (GIT_SHA1, GIT_SHA1_SHORT, GIT_DATE, GIT_TAG) with information from git
macro(get_git_info)
    find_package(Git)
    if(Git_FOUND)
        message(STATUS "Git found: ${GIT_EXECUTABLE}")
    endif()

    # the commit's SHA1, and whether the building workspace was dirty or not
    execute_process(COMMAND
        "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(COMMAND
        "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_SHA1_SHORT
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

    # the date of the commit
    execute_process(COMMAND
        "${GIT_EXECUTABLE}" log -1 --format=%ad --date=iso
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_DATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND
        "${GIT_EXECUTABLE}" describe --abbrev=0 --tags
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_TAG
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endmacro()

# configure version.in file with version and git informations
macro(configure_version_file)
    set(PRODUCT_VERSION ${CMAKE_PROJECT_VERSION})
    set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
    string(SUBSTRING ${GIT_DATE} 0 4 BUILD_YEAR)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/version.h" @ONLY)
endmacro()

get_git_info()

# parse the version information from tag into pieces. search for v(number).(number).(number)
string(REGEX REPLACE "[a-zA-Z_]*\\v([0-9]+)\\..*" "\\1" PROJECT_VERSION_MAJOR "${GIT_TAG}")
string(REGEX REPLACE "[a-zA-Z_]*\\v[0-9]+\\.([0-9]+).*" "\\1" PROJECT_VERSION_MINOR "${GIT_TAG}")
string(REGEX REPLACE "[a-zA-Z_]*\\v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" PROJECT_VERSION_PATCH "${GIT_TAG}")
set(PROJECT_VERSION_TWEAK 0)
message(STATUS "version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# generate a qt deployed environment as a postbuild comd
macro(generate_qtdeployment_as_postbuild APPLICATION_NAME)
    string(CONCAT WinDeployQtPath  ${BVIA_BUILD_LIBS_PATH} "\\Qt-5.13.2\\5.13.2\\msvc2017_64\\bin\\windeployqt.exe")
    add_custom_command(
        TARGET ${APPLICATION_NAME}
        POST_BUILD
            COMMAND ${WinDeployQtPath} --no-patchqt --no-angle --no-opengl-sw --dir $(OutDir) $(TargetPath)
)
endmacro()
