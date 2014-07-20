find_package(ProtobufPlugin REQUIRED)
include_directories($PROTOBUF_INCLUDE_DIR)

set(ZQRPC_SRCS
	FileGenerator.cc
	ServiceGenerator.cc
	ZqrpcCppGenerator.cc
	ZqrpcCppMain.cc
)
add_executable(protoc-gen-cpp_zqrpc ${ZQRPC_SRCS})
target_link_libraries(protoc-gen-cpp_zqrpc ${PROTOBUF_LIBRARY} ${PROTOBUF_PROTOC_LIBRARY})
install(TARGETS protoc-gen-cpp_zqrpc
    RUNTIME DESTINATION bin)
