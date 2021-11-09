cmake_minimum_required ( VERSION 3.11 )

project(fwmux)

#include some helping modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

#include(FindPythonLibs)
include(GenConfigH)

#check libraries
#find_package(LibEvent REQUIRED)
#find_package(LibCrypt REQUIRED)


#define current version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 8)
set(VERSION_REVISION 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION})

set(ARBITRARY_LOGFILES ON CACHE BOOL "Should Enhanced Logging be enabled")
set(SQL_SUPPORT OFF CACHE BOOL "Should SQL Support be enabled")
set(MOVE_HELP OFF CACHE BOOL "Should we rename the 'help' command to 'muxhelp'")
set(USE_PYTHON ON CACHE BOOL "Use Python bindings")

set(PACKAGE_STRING "FoxWell MUX")

if(WIN32)
    #generate config.h
    #gen_config_h()
    configure_file(${CMAKE_CURRENT_LIST_DIR}/src/autoconf-win32.h ${CMAKE_CURRENT_BINARY_DIR}/src/autoconf.h COPYONLY)

    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/autoconf.h)

    endif()

endif()



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)


