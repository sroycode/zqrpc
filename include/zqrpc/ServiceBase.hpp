#ifndef _ZQRPC_SERVICE_HPP_
#define _ZQRPC_SERVICE_HPP_

#include <assert.h>
#include <string>

namespace google {
namespace protobuf {
class Message;
class MethodDescriptor;
class ServiceDescriptor;
}  // namespace protobuf
}  // namespace google

namespace zqrpc {
class ServiceBase;
}

namespace zqrpc {

class ServiceBase {
 public:
  ServiceBase();

  virtual ~ServiceBase();

  virtual const ::google::protobuf::ServiceDescriptor* GetDescriptor() = 0;

  virtual const ::google::protobuf::Message& GetRequestPrototype(
      const ::google::protobuf::MethodDescriptor*) const = 0;
  virtual const ::google::protobuf::Message& GetResponsePrototype(
      const ::google::protobuf::MethodDescriptor*) const = 0;

  virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response)=0;
};
}  // namespace
#endif
