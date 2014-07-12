include(zqrpc_functions)
find_package(ProtobufPlugin REQUIRED)
PROTOBUF_GENERATE_CPP(ECHO_PB_SRCS ECHO_PB_HDRS echo.proto)
PROTOBUF_GENERATE_ZQRPC(ECHO_ZQRPC_SRCS ECHO_ZQRPC_HDRS echo.proto)

add_library(echo_pb ${ECHO_PB_SRCS} ${ECHO_PB_HDRS})
#add_library(echo_zqrpc ${ECHO_ZQRPC_SRCS} ${ECHO_ZQRPC_HDRS})

target_link_libraries(echo_pb ${PROTOBUF_LIBRARY})
include_directories(${PROJECT_BINARY_DIR}/examples)

add_executable(EchoClient EchoClient.cc)
target_link_libraries(EchoClient zqrpc echo_pb)

add_executable(EchoServer EchoServer.cc)
target_link_libraries(EchoServer zqrpc echo_pb)
