set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  #set(CMAKE_BUILD_TYPE "Release")
endif()
message("-- Build type: ${CMAKE_BUILD_TYPE}")

option(zqrpc_build_plugin "Build zqrpc's plugin." OFF)
option(zqrpc_build_tests "Build zqrpc's tests." OFF)
option(zqrpc_build_static "Build static library." OFF)
option(zqrpc_build_examples "Build zqrpc's examples." ON)

find_package(GLog REQUIRED)
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
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Werror")
set(CMAKE_OSX_ARCHITECTURES x86_64)

add_subdirectory(src)

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

file(GLOB ZQRPC_PUBLIC_HEADERS
	"${PROJECT_SOURCE_DIR}/include/zqrpc/*.h*"
	"${PROJECT_SOURCE_DIR}/include/zqrpc.hpp"
)
install(FILES ${ZQRPC_PUBLIC_HEADERS} DESTINATION include)
