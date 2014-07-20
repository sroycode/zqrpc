
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include "ServiceGenerator.hh"
#include "strutil.h"
#include "cpp_helpers.h"

using namespace google::protobuf::compiler::cpp;

zqrpc::plugin::ServiceGenerator::ServiceGenerator(const google::protobuf::ServiceDescriptor* descriptor,
                                   const std::string& dllexport_decl)
	: descriptor_(descriptor)
{
	vars_["classname"] = descriptor_->name();
	vars_["full_name"] = descriptor_->full_name();
	if (dllexport_decl.empty()) {
		vars_["dllexport"] = "";
	} else {
		vars_["dllexport"] = dllexport_decl + " ";
	}
}

zqrpc::plugin::ServiceGenerator::~ServiceGenerator() {}

void zqrpc::plugin::ServiceGenerator::GenerateDeclarations(google::protobuf::io::Printer* printer)
{
	// Forward-declare the stub type.
	printer->Print(vars_,
	               "class $classname$_Stub;\n"
	               "\n");

	GenerateInterface(printer);
	GenerateStubDefinition(printer);
}

void zqrpc::plugin::ServiceGenerator::GenerateInterface(google::protobuf::io::Printer* printer)
{
	printer->Print(vars_,
	               "class $dllexport$$classname$ : public zqrpc::ServiceBase {\n"
	               " protected:\n"
	               "  // This class should be treated as an abstract interface.\n"
	               "  inline $classname$() {};\n"
	               " public:\n"
	               "  virtual ~$classname$();\n");
	printer->Indent();

	printer->Print(vars_,
	               "\n"
	               "typedef $classname$_Stub Stub;\n"
	               "\n"
	               "static const ::google::protobuf::ServiceDescriptor* descriptor();\n"
	               "\n");

	GenerateMethodSignatures(VIRTUAL, printer, false);

	printer->Print(
	    "\n"
	    "// implements Service ----------------------------------------------\n"
	    "\n"
	    "const ::google::protobuf::ServiceDescriptor* GetDescriptor();\n"
	    "void CallMethod(const ::google::protobuf::MethodDescriptor* method,\n"
	    "                const ::google::protobuf::Message* request,\n"
	    "                ::google::protobuf::Message* response);\n"
	    "const ::google::protobuf::Message& GetRequestPrototype(\n"
	    "  const ::google::protobuf::MethodDescriptor* method) const;\n"
	    "const ::google::protobuf::Message& GetResponsePrototype(\n"
	    "  const ::google::protobuf::MethodDescriptor* method) const;\n");

	printer->Outdent();
	printer->Print(vars_,
	               "\n"
	               " private:\n"
	               "  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($classname$);\n"
	               "};\n"
	               "\n");
}

void zqrpc::plugin::ServiceGenerator::GenerateStubDefinition(google::protobuf::io::Printer* printer)
{
	printer->Print(vars_,
	               "class $dllexport$$classname$_Stub {\n"
	               " public:\n");

	printer->Indent();

	printer->Print(vars_,
	               "$classname$_Stub(::zqrpc::RpcChannel* channel); \n"
	               "~$classname$_Stub();\n"
	               "\n"
	               "inline ::zqrpc::RpcChannel* channel() { return channel_; }\n"
	               "\n"
	               "\n");

	GenerateMethodSignatures(NON_VIRTUAL, printer, true);

	printer->Outdent();
	printer->Print(vars_,
	               " private:\n"
	               "  ::zqrpc::RpcChannel* channel_;\n"
	               "  bool owns_channel_;\n"
	               "  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($classname$_Stub);\n"
	               "};\n"
	               "\n");
}

void zqrpc::plugin::ServiceGenerator::GenerateMethodSignatures(
    VirtualOrNon virtual_or_non, google::protobuf::io::Printer* printer, bool stub)
{
	for (int i = 0; i < descriptor_->method_count(); i++) {
		const google::protobuf::MethodDescriptor* method = descriptor_->method(i);
		std::map<std::string, std::string> sub_vars;
		sub_vars["name"] = method->name();
		sub_vars["input_type"] = ClassName(method->input_type(), true);
		sub_vars["output_type"] = ClassName(method->output_type(), true);
		sub_vars["virtual"] = virtual_or_non == VIRTUAL ? "virtual " : "";

		if (stub) {
      printer->Print(sub_vars,
                     "$virtual$void $name$(::zqrpc::ZController* controller,\n"
                     "                     $input_type$* request,\n"
                     "                     $output_type$* response,\n"
                     "                     long deadline_ms=-1);\n"
                     "$virtual$void $name$_Send(::zqrpc::ZController* controller,\n"
                     "                     $input_type$* request);\n"
                     "$virtual$void $name$_Recv(::zqrpc::ZController* controller,\n"
                     "                     $output_type$* response,\n"
                     "                     long deadline_ms=-1);\n");
		} else {
			printer->Print(
			    sub_vars,
			    "$virtual$void $name$(const $input_type$* request,\n"
			    "                     $output_type$* response);\n");
		}
	}
}

// ===================================================================

void zqrpc::plugin::ServiceGenerator::GenerateDescriptorInitializer(
    google::protobuf::io::Printer* printer, int index)
{
	std::map<std::string, std::string> vars;
	vars["classname"] = descriptor_->name();
	vars["index"] = google::protobuf::SimpleItoa(index);

	printer->Print(vars,
	               "$classname$_descriptor_ = file->service($index$);\n");
}

// ===================================================================

void zqrpc::plugin::ServiceGenerator::GenerateImplementation(google::protobuf::io::Printer* printer)
{
	printer->Print(vars_,
	               "$classname$::~$classname$() {}\n"
	               "\n"
	               "const ::google::protobuf::ServiceDescriptor* $classname$::descriptor() {\n"
	               "  protobuf_AssignDescriptorsOnce();\n"
	               "  return $classname$_descriptor_;\n"
	               "}\n"
	               "\n"
	               "const ::google::protobuf::ServiceDescriptor* $classname$::GetDescriptor() {\n"
	               "  protobuf_AssignDescriptorsOnce();\n"
	               "  return $classname$_descriptor_;\n"
	               "}\n"
	               "\n");

	// Generate methods of the interface.
	GenerateNotImplementedMethods(printer);
	GenerateCallMethod(printer);
	GenerateGetPrototype(REQUEST, printer);
	GenerateGetPrototype(RESPONSE, printer);

	// Generate stub implementation.
	printer->Print(vars_,
	               "$classname$_Stub::$classname$_Stub(::zqrpc::RpcChannel* channel)\n"
	               "  : channel_(channel) {}\n"
	               "$classname$_Stub::~$classname$_Stub() {}\n"
	               "\n");

	GenerateStubMethods(printer);
}

void zqrpc::plugin::ServiceGenerator::GenerateNotImplementedMethods(google::protobuf::io::Printer* printer)
{
	for (int i = 0; i < descriptor_->method_count(); i++) {
		const google::protobuf::MethodDescriptor* method = descriptor_->method(i);
		std::map<std::string, std::string> sub_vars;
		sub_vars["classname"] = descriptor_->name();
		sub_vars["name"] = method->name();
		sub_vars["index"] = google::protobuf::SimpleItoa(i);
		sub_vars["input_type"] = ClassName(method->input_type(), true);
		sub_vars["output_type"] = ClassName(method->output_type(), true);

		printer->Print(sub_vars,
		               "void $classname$::$name$(const $input_type$* request,\n"
		               "                         $output_type$* response) {\n"
		               "  throw ::zqrpc::ZError(::zqrpc::ZEC_METHODNOTIMPLEMENTED,\n"
		               "              \"Method $name$() not implemented.\");\n"
		               "}\n"
		               "\n");
	}
}

void zqrpc::plugin::ServiceGenerator::GenerateCallMethod(google::protobuf::io::Printer* printer)
{
	printer->Print(vars_,
	               "void $classname$::CallMethod(const ::google::protobuf::MethodDescriptor* method,\n"
	               "                             const ::google::protobuf::Message* request,\n"
	               "                             ::google::protobuf::Message* response) {\n"
	               "  GOOGLE_DCHECK_EQ(method->service(), $classname$_descriptor_);\n"
	               "  switch(method->index()) {\n");

	for (int i = 0; i < descriptor_->method_count(); i++) {
		const google::protobuf::MethodDescriptor* method = descriptor_->method(i);
		std::map<std::string, std::string> sub_vars;
		sub_vars["name"] = method->name();
		sub_vars["index"] = google::protobuf::SimpleItoa(i);
		sub_vars["input_type"] = ClassName(method->input_type(), true);
		sub_vars["output_type"] = ClassName(method->output_type(), true);

		printer->Print(sub_vars,
		               "    case $index$:\n"
		               "      $name$(\n"
		               "          ::google::protobuf::down_cast<const $input_type$*>(request),\n"
		               "          ::google::protobuf::down_cast< $output_type$* >(response) );\n"
		               "      break;\n");
	}

	printer->Print(vars_,
	               "    default:\n"
	               "      GOOGLE_LOG(FATAL) << \"Bad method index; this should never happen.\";\n"
	               "      break;\n"
	               "  }\n"
	               "}\n"
	               "\n");
}

void zqrpc::plugin::ServiceGenerator::GenerateGetPrototype(RequestOrResponse which,
        google::protobuf::io::Printer* printer)
{
	if (which == REQUEST) {
		printer->Print(vars_,
		               "const ::google::protobuf::Message& $classname$::GetRequestPrototype(\n");
	} else {
		printer->Print(vars_,
		               "const ::google::protobuf::Message& $classname$::GetResponsePrototype(\n");
	}

	printer->Print(vars_,
	               "    const ::google::protobuf::MethodDescriptor* method) const {\n"
	               "  GOOGLE_DCHECK_EQ(method->service(), descriptor());\n"
	               "  switch(method->index()) {\n");

	for (int i = 0; i < descriptor_->method_count(); i++) {
		const google::protobuf::MethodDescriptor* method = descriptor_->method(i);
		const google::protobuf::Descriptor* type =
		    (which == REQUEST) ? method->input_type() : method->output_type();

		std::map<std::string, std::string> sub_vars;
		sub_vars["index"] = google::protobuf::SimpleItoa(i);
		sub_vars["type"] = ClassName(type, true);

		printer->Print(sub_vars,
		               "    case $index$:\n"
		               "      return $type$::default_instance();\n");
	}

	printer->Print(vars_,
	               "    default:\n"
	               "      GOOGLE_LOG(FATAL) << \"Bad method index; this should never happen.\";\n"
	               "      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);\n"
	               "  }\n"
	               "}\n"
	               "\n");
}

void zqrpc::plugin::ServiceGenerator::GenerateStubMethods(google::protobuf::io::Printer* printer)
{
	for (int i = 0; i < descriptor_->method_count(); i++) {
		const google::protobuf::MethodDescriptor* method = descriptor_->method(i);
		std::map<std::string, std::string> sub_vars;
		sub_vars["classname"] = descriptor_->name();
		sub_vars["name"] = method->name();
		sub_vars["index"] = google::protobuf::SimpleItoa(i);
		sub_vars["input_type"] = ClassName(method->input_type(), true);
		sub_vars["output_type"] = ClassName(method->output_type(), true);

		printer->Print(sub_vars,
      "void $classname$_Stub::$name$(::zqrpc::ZController* controller,\n"
      "                     $input_type$* request,\n"
      "                     $output_type$* response,\n"
      "                     long deadline_ms){\n"
      "  $name$_Send(controller,request);\n"
      "  $name$_Recv(controller,response,deadline_ms);\n"
		  "}\n");
		printer->Print(sub_vars,
      "void $classname$_Stub::$name$_Send(::zqrpc::ZController* controller,\n"
      "                     $input_type$* request){\n"
      "  channel_->ZSendMethod($classname$::descriptor()->method($index$),\n"
      "                        controller,\n"
      "                        request);\n"
		  "}\n");
		printer->Print(sub_vars,
      "void $classname$_Stub::$name$_Recv(::zqrpc::ZController* controller,\n"
      "                     $output_type$* response,\n"
      "                     long deadline_ms){\n"
      "  channel_->ZRecvMethod(controller,\n"
      "                        response,\n"
      "                        deadline_ms);\n"
		  "}\n");
	}
}

