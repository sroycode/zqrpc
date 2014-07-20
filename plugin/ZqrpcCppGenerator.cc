#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <string>
#include <utility>
#include <vector>

#include "FileGenerator.hh"
#include "ZqRpcCppGenerator.hh"

namespace zqrpc {
namespace plugin {

namespace {
inline bool HasSuffixString(const std::string& str,
                            const std::string& suffix)
{
	return str.size() >= suffix.size() &&
	       str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::string StripSuffixString(const std::string& str, const std::string& suffix)
{
	if (HasSuffixString(str, suffix)) {
		return str.substr(0, str.size() - suffix.size());
	} else {
		return str;
	}
}
}
}  // namespace plugin
}  // namespace zqrpc

zqrpc::plugin::ZqrpcCppGenerator::ZqrpcCppGenerator() {}
zqrpc::plugin::ZqrpcCppGenerator::~ZqrpcCppGenerator() {}
bool zqrpc::plugin::ZqrpcCppGenerator::Generate(
    const ::google::protobuf::FileDescriptor* file,
    const std::string& parameter,
    ::google::protobuf::compiler::GeneratorContext* generator_context,
    std::string* error) const
{
	std::vector<std::pair<std::string, std::string> > options;
	::google::protobuf::compiler::ParseGeneratorParameter(parameter, &options);

	// If the dllexport_decl option is passed to the compiler, we need to write
	// it in front of every symbol that should be exported if this .proto is
	// compiled into a Windows DLL.  E.g., if the user invokes the protocol
	// compiler as:
	//   protoc --cpp_out=dllexport_decl=FOO_EXPORT:outdir foo.proto
	// then we'll define classes like this:
	//   class FOO_EXPORT Foo {
	//     ...
	//   }
	// FOO_EXPORT is a macro which should expand to __declspec(dllexport) or
	// __declspec(dllimport) depending on what is being compiled.
	std::string dllexport_decl;

	for (size_t i = 0; i < options.size(); i++) {
		if (options[i].first == "dllexport_decl") {
			dllexport_decl = options[i].second;
		} else {
			*error = "Unknown generator option: " + options[i].first;
			return false;
		}
	}

	// -----------------------------------------------------------------


	std::string basename = StripSuffixString(file->name(), ".proto");
	basename.append(".zqrpc");

	FileGenerator file_generator(file, dllexport_decl);

	// Generate header.
	{
		::google::protobuf::scoped_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
		    generator_context->Open(basename + ".h"));
		::google::protobuf::io::Printer printer(output.get(), '$');
		file_generator.GenerateHeader(&printer);
	}

	// Generate cc file.
	{
		::google::protobuf::scoped_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
		    generator_context->Open(basename + ".cc"));
		::google::protobuf::io::Printer printer(output.get(), '$');
		file_generator.GenerateSource(&printer);
	}

	return true;
}
