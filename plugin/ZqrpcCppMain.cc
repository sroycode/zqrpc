#include <google/protobuf/compiler/plugin.h>
#include "ZqrpcCppGenerator.hh"

int main(int argc, char* argv[])
{
	zqrpc::plugin::ZqrpcCppGenerator generator;
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
