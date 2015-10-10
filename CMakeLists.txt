# compiler version check
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" CMAKE_COMPILER_SUPPORTS_CXX11)
set(CMAKE_COMPILER_SUPPORTS_CXX11 OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set (CMAKE_COMPILER_IS_CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set (CMAKE_COMPILER_IS_GNUCC 1)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (DEBUGMODE ON)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    # cmake bug workaround -- on some platforms, cmake doesn't set NDEBUG for RelWithDebInfo mode
    add_definitions ("-DNDEBUG")
endif ()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message (STATUS "Project source dir = ${PROJECT_SOURCE_DIR}")
message (STATUS "Project build dir = ${CMAKE_BINARY_DIR}")

if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message (FATAL_ERROR "Not allowed to run in-source build!")
endif ()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)

option(zqrpc_build_plugin "Build zqrpc's plugin." ON)
option(zqrpc_build_tests "Build zqrpc's tests." OFF)
option(zqrpc_build_static "Build static library." OFF)
option(zqrpc_build_examples "Build zqrpc's examples." ON)

find_package(Glog REQUIRED)
find_package(ProtobufPlugin REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread program_options system)
find_package(ZeroMQ REQUIRED)

include_directories(${GLOG_INCLUDE_DIRS})
include_directories(${ZeroMQ_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/src)

cmake_minimum_required (VERSION 2.8.0)

if(CMAKE_COMPILER_SUPPORTS_CXX11)
	add_definitions ("-std=c++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCOMPILER_SUPPORTS_CXX11=1")
endif()

# Clang-specific options
if (CMAKE_COMPILER_IS_CLANG OR CMAKE_COMPILER_IS_APPLECLANG)
    add_definitions ("-Wno-unused-function")
    add_definitions ("-Wno-overloaded-virtual")
    add_definitions ("-Wno-unneeded-internal-declaration")
    add_definitions ("-Wno-unused-private-field")
    add_definitions ("-Wno-tautological-compare")
    # disable warning about unused command line arguments
    add_definitions ("-Qunused-arguments")
    # Don't warn if we ask it not to warn about warnings it doesn't know
    add_definitions ("-Wunknown-warning-option")
    if (CLANG_VERSION_STRING VERSION_GREATER 3.5 OR APPLECLANG_VERSION_STRING VERSION_GREATER 6.0)
        add_definitions ("-Wno-unused-local-typedefs")
    endif ()
elseif(CMAKE_COMPILER_IS_GNUCC)
    if (NOT ${GCC_VERSION} VERSION_LESS 4.8)
        add_definitions("-Wno-error=unused-local-typedefs")
        add_definitions("-Wno-unused-local-typedefs")
    endif ()
endif ()

set(CMAKE_OSX_ARCHITECTURES x86_64)

add_subdirectory(src)
add_subdirectory(simple)

if(zqrpc_build_plugin)
  add_subdirectory(plugin)
endif(zqrpc_build_plugin)

if(zqrpc_build_tests)
  enable_testing()
  add_subdirectory(test)
endif(zqrpc_build_tests)

if(zqrpc_build_examples)
  add_subdirectory(examples)
endif(zqrpc_build_examples)

install(FILES include/zqrpc.hpp DESTINATION include)
install(DIRECTORY include/zqrpc DESTINATION include)
