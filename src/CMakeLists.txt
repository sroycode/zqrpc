project (ZQRPC)

# The version number.
set (ZQRPC_VERSION_MAJOR 0)
set (ZQRPC_VERSION_MINOR 1)

# add_subdirectory(plugin)

include_directories(${PROJECT_BINARY_DIR})
set(ZQRPC_SOURCES
    RpcChannel.cc
    RpcServer.cc
    RpcWorker.cc
    ServiceBase.cc)

set(ZQRPC_LIB_DEPS 
    ${GLOG_LIBRARIES}
    ${ZeroMQ_LIBRARIES}
    ${PROTOBUF_LIBRARY}
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_THREAD_LIBRARIES})

add_library(zqrpc SHARED ${ZQRPC_SOURCES})
target_link_libraries(zqrpc ${ZQRPC_LIB_DEPS})
set(ZQRPC_TARGET_LIBS zqrpc)

if(zqrpc_build_static)
    add_library(zqrpc_static STATIC ${ZQRPC_SOURCES})
    target_link_libraries(zqrpc_static ${ZQRPC_LIB_DEPS})
    set_target_properties(zqrpc_static PROPERTIES OUTPUT_NAME zqrpc)
    list(APPEND ZQRPC_TARGET_LIBS zqrpc_static)
endif(zqrpc_build_static)

install(TARGETS ${ZQRPC_TARGET_LIBS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
