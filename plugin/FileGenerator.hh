#ifndef _ZQRPC_FILE_GENERATOR_HH_
#define _ZQRPC_FILE_GENERATOR_HH_

#include <string>
#include <vector>

#include "ServiceGenerator.hh"

namespace zqrpc {
namespace plugin {

class ServiceGenerator;
}  // namespace plugin
}  // namespace zqrpc

namespace google {
namespace protobuf {
class FileDescriptor;
class ServiceDescriptor;

namespace io {
class Printer;
}
}
}

namespace zqrpc {
namespace plugin {

class FileGenerator {
public:
	FileGenerator(const google::protobuf::FileDescriptor* file, const std::string& dllexport_decl);

	~FileGenerator();

	void GenerateHeader(google::protobuf::io::Printer* printer);

	void GenerateSource(google::protobuf::io::Printer* printer);

private:
	void GenerateNamespaceOpeners(google::protobuf::io::Printer* printer);

	void GenerateNamespaceClosers(google::protobuf::io::Printer* printer);

	void GenerateBuildDescriptors(google::protobuf::io::Printer* printer);

	std::vector<std::string> package_parts_;
	std::vector<ServiceGenerator*> service_generators_;
	const ::google::protobuf::FileDescriptor* file_;
	std::string dllexport_decl_;
};

}  // namespace plugin
}  // namespace zqrpc
#endif
