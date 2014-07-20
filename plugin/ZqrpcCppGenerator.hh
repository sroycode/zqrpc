#ifndef _ZQRPC_CPP_GENERATOR_HH_
#define _ZQRPC_CPP_GENERATOR_HH_

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/stubs/common.h>
#include <string>

namespace google {
namespace protobuf {
class FileDescriptor;
}  // namespace protobuf
}  // namespace google

namespace zqrpc {
namespace plugin {

class LIBPROTOC_EXPORT ZqrpcCppGenerator :
	public ::google::protobuf::compiler::CodeGenerator {
public:
	ZqrpcCppGenerator();
	~ZqrpcCppGenerator();

	bool Generate(
	    const ::google::protobuf::FileDescriptor* file,
	    const ::std::string& parameter,
	    ::google::protobuf::compiler::GeneratorContext* generator_context,
	    std::string* error) const;

private:
	GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ZqrpcCppGenerator);
};

}  // namespace plugin
}  // namespace zqrpc
#endif
