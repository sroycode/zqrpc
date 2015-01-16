/**
 * @project zqrpc
 * @file include/zqrpc/ServiceBase.hpp
 * @author  S Roychowdhury <sroycode @ gmail DOT com>
 * @version 0.1
 *
 * @section LICENSE
 *
 * Copyright (c) 2014 S Roychowdhury
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 *  ServiceBase.hpp : Base class for rpc service
 *
 */
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
