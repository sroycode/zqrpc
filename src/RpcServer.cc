/**
 * @project zqrpc
 * @file src/RpcServer.cc
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
 *  RpcServer.cc : Server Implemantation
 *
 */
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include "zqrpc/RpcHeaders.hh"
#include "zqrpc/RpcServer.hh"
#include "zqrpc/RpcWorker.hh"

zqrpc::RpcServer::RpcServer(zmq::context_t* context) :
	context_(context),
	rpc_frontend_(context_,ZMQ_ROUTER,"ROUTER"),
	rpc_backend_(context_,ZMQ_DEALER,"DEALER"),
	started_(false)
{
}

zqrpc::RpcServer::~RpcServer()
{
	Close();
}

void zqrpc::RpcServer::EndPoint(const char* url)
{
	rpc_bind_vec_.push_back(std::string(url));
}

void zqrpc::RpcServer::RegisterService(zqrpc::ServiceBase* service)
{
	const google::protobuf::ServiceDescriptor *descriptor = service->GetDescriptor();
	for (int i = 0; i < descriptor->method_count(); ++i) {
		const google::protobuf::MethodDescriptor *method = descriptor->method(i);
		const google::protobuf::Message *request = &service->GetRequestPrototype(method);
		const google::protobuf::Message *response = &service->GetResponsePrototype(method);
		RpcMethod *rpc_method = new RpcMethod(service, request, response, method);
		std::string opcode=std::string(method->full_name());
		// uint32_t opcode = ZQRPC_HASHFN(methodname.c_str(),methodname.length());
		RpcMethodMapT::const_iterator iter = rpc_method_map_.find(opcode);
		if (iter == rpc_method_map_.end())
			rpc_method_map_[opcode] = rpc_method;
	}
}

void zqrpc::RpcServer::RemoveService(zqrpc::ServiceBase* service)
{
	const google::protobuf::ServiceDescriptor *descriptor = service->GetDescriptor();
	for (int i = 0; i < descriptor->method_count(); ++i) {
		const google::protobuf::MethodDescriptor *method = descriptor->method(i);
		std::string opcode=std::string(method->full_name());
		// uint32_t opcode = ZQRPC_HASHFN(methodname.c_str(),methodname.length());
		RpcMethodMapT::iterator iter = rpc_method_map_.find(opcode);
		if (iter != rpc_method_map_.end()) delete iter->second;
	}
}

void zqrpc::RpcServer::Start(std::size_t noof_threads)
{
	boost::thread_group threads;
	try {
		for (RpcBindVecT::const_iterator it = rpc_bind_vec_.begin(); it!=rpc_bind_vec_.end();++it) {
			rpc_frontend_.bind(it->c_str());
		}
		rpc_backend_.bind(ZQRPC_INPROC_WORKER);
		// set started flag here
		started_=true;

		for(std::size_t i = 0; i < noof_threads; ++i) {
			zqrpc::RpcWorker w(context_, rpc_method_map_);
			threads.create_thread<zqrpc::RpcWorker>(w);
		}
		rpc_frontend_.ProxyTo( rpc_backend_);
		DLOG(INFO) << "Loop Ended " << std::endl;
	} catch(const zmq::error_t& e) {
		DLOG(INFO) << "Start Exception: " << e.what() << std::endl;
	}
	threads.join_all();
}

void zqrpc::RpcServer::Close()
{
	for (RpcMethodMapT::iterator it = rpc_method_map_.begin(); it != rpc_method_map_.end();) {
		RpcMethod *rpc_method = it->second;
		++it;
		delete rpc_method;
	}
	if (started_) {
		for (RpcBindVecT::iterator it = rpc_bind_vec_.begin(); it != rpc_bind_vec_.end();) {
			rpc_frontend_.unbind((*it).c_str());
		}
		rpc_backend_.unbind(ZQRPC_INPROC_WORKER);
	}
	rpc_frontend_.close();
	rpc_backend_.close();
}
