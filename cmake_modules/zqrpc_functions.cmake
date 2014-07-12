set(ZQRPC_PLUGIN_ROOT ${CMAKE_CURRENT_LIST_DIR}/../build/plugin)

function(PROTOBUF_GENERATE_ZQRPC SRCS HDRS)
  set(PLUGIN_BIN ${ZQRPC_PLUGIN_ROOT}/protoc-gen-cpp_zqrpc)
  PROTOBUF_GENERATE_MULTI(PLUGIN "cpp_zqrpc" PROTOS ${ARGN}
                          OUTPUT_STRUCT "_SRCS:.zqrpc.cc;_HDRS:.zqrpc.h"
                          FLAGS "--plugin=protoc-gen-cpp_zqrpc=${PLUGIN_BIN}"
                          DEPENDS ${PLUGIN_BIN})
  set(${SRCS} ${_SRCS} PARENT_SCOPE)
  set(${HDRS} ${_HDRS} PARENT_SCOPE)
endfunction()
