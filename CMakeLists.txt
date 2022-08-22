cmake_minimum_required(VERSION 3.16)

project(rtklib LANGUAGES C CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message("OS Darwin detected")
    set(OSX_ARCHITECTURES arm64)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(RTKLIB_PYTHON_C_MODULE_NAME "_rtklib" CACHE STRING "Name of the C extension module file")

set(RTKLIB_SRCS
        "src/convkml.c"
        "src/convrnx.c"
        "src/datum.c"
        "src/download.c"
        "src/ephemeris.c"
        "src/geoid.c"
        "src/ionex.c"
        "src/lambda.c"
        "src/options.c"
        "src/pntpos.c"
        "src/postpos.c"
        "src/ppp.c"
        "src/ppp_ar.c"
        "src/preceph.c"
        "src/qzslex.c"
        "src/rcvraw.c"
        "src/rinex.c"
        "src/rtcm.c"
        "src/rtcm2.c"
        "src/rtcm3.c"
        "src/rtcm3e.c"
        "src/rtkcmn.c"
        "src/rtkpos.c"
        "src/rtksvr.c"
        "src/sbas.c"
        "src/solution.c"
        "src/stream.c"
        "src/streamsvr.c"
        "src/tle.c"
        "src/rcv/binex.c"
        "src/rcv/crescent.c"
        "src/rcv/gw10.c"
        "src/rcv/javad.c"
        "src/rcv/novatel.c"
        "src/rcv/nvs.c"
        "src/rcv/rcvlex.c"
        "src/rcv/rt17.c"
        "src/rcv/septentrio.c"
        "src/rcv/skytraq.c"
        "src/rcv/ss2.c"
        "src/rcv/ublox.c"
        )

add_library(rtklibStatic STATIC ${RTKLIB_SRCS})
target_include_directories(rtklibStatic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

set(Python_FIND_VIRTUALENV FIRST)
find_package(PythonInterp 3 REQUIRED)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE}
    "-cimport sysconfig; print(sysconfig.get_paths()['include'])"
    OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS
    RESULT_VARIABLE PYTHON_INCLUDE_DIRS_NOT_FOUND
)

if(PYTHON_INCLUDE_DIRS_NOT_FOUND)
    message(FATAL_ERROR "Python headers not found")
endif()
find_package(PythonLibs 3 REQUIRED)

if(WIN32)
    find_package(PythonLibs 3 REQUIRED)
endif()

add_subdirectory(python_extensions)
