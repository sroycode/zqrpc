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
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include "zqrpc/RpcHeaders.hh"
#include "zqrpc/RpcServer.hh"
#include "zqrpc/RpcWorker.hh"



zqrpc::RpcServer::RpcServer(zmq::context_t* context,bool noreply, const char* suffix) :
	context_(context),
	suffix_(suffix),
	use_inproc_workil( std::string(ZQRPC_INPROC_WORKIL)+std::string(suffix_) ),
	use_inproc_worker( std::string(ZQRPC_INPROC_WORKER)+std::string(suffix_) ),
	use_inproc_pcontrol( std::string(ZQRPC_INPROC_PCONTROL)+std::string(suffix_) ),
	rpc_frontend_(context_, noreply ? ZMQ_PUB : ZMQ_ROUTER,"ROUTER"),
	rpc_backend_(context_,ZMQ_DEALER,"DEALER"),
	rpc_control_(context_,ZMQ_DEALER,"CONTROL"),
	started_(0),
	noreply_(noreply)
{
}

zqrpc::RpcServer::RpcServer(zmq::context_t* context,const char* suffix) :
	context_(context),
	suffix_(suffix),
	use_inproc_workil( std::string(ZQRPC_INPROC_WORKIL)+std::string(suffix_) ),
	use_inproc_worker( std::string(ZQRPC_INPROC_WORKER)+std::string(suffix_) ),
	use_inproc_pcontrol( std::string(ZQRPC_INPROC_PCONTROL)+std::string(suffix_) ),
	rpc_frontend_(context_,ZMQ_ROUTER,"ROUTER"),
	rpc_backend_(context_,ZMQ_DEALER,"DEALER"),
	rpc_control_(context_,ZMQ_DEALER,"CONTROL"),
	started_(0),
	noreply_(false)
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
	try {
		for (RpcBindVecT::const_iterator it = rpc_bind_vec_.begin(); it!=rpc_bind_vec_.end(); ++it) {
			rpc_frontend_.bind(it->c_str());
		}
		rpc_frontend_.bind(use_inproc_workil.c_str());
		DLOG(INFO) << "Bound " << use_inproc_workil.c_str() << std::endl;
		rpc_backend_.bind(use_inproc_worker.c_str());
		DLOG(INFO) << "Bound " << use_inproc_worker.c_str() << std::endl;
		rpc_control_.bind(use_inproc_pcontrol.c_str());
		DLOG(INFO) << "Bound " << use_inproc_pcontrol.c_str() << std::endl;
		
		started_=true;
		for(std::size_t i = 0; i < noof_threads; ++i) {
			threads_.push_back( new boost::thread(zqrpc::RpcWorker(context_, rpc_method_map_,noreply_, suffix_)) );
		}
		ProxyStart();
		DLOG(INFO) << "Loop Ended " << std::endl;
	} catch(const ProxyException& e) {
		DLOG(INFO) << "Proxy Exception: " << std::endl;
	} catch(const zmq::error_t& e) {
		DLOG(INFO) << "Start Exception: " << e.what() << std::endl;
	}
}

void zqrpc::RpcServer::Close()
{
	/** 
	// delete all methods
	for (RpcMethodMapT::iterator it = rpc_method_map_.begin(); it != rpc_method_map_.end();) {
		RpcMethod *rpc_method = it->second;
		++it;
		delete rpc_method;
	}
	*/
	// unbind , inprocs cannot be unbound
	if (started_) {
		DLOG(INFO) << "server close called, started " << started_ << std::endl;
		for (RpcBindVecT::iterator it = rpc_bind_vec_.begin(); it != rpc_bind_vec_.end(); ++it) {
			rpc_frontend_.unbind((*it).c_str());
		}
		started_=false;
		DLOG(INFO) << "stopping workers " << threads_.size() << std::endl;
		WorkerStop( threads_.size() );
		
		int jc=0;
		DLOG(INFO) << "joining threads " << jc << std::endl;
		for (zqrpc::RpcServer::ThreadPtrVecT::iterator it = threads_.begin();it!=threads_.end();) {
			DLOG(INFO) << "joining threads " << ++jc << std::endl;
			boost::thread* t = *it;
			++it;
			t->join();
		}
		DLOG(INFO) << "proxy terminate called" << std::endl;
		ProxyStop();
		DLOG(INFO) << "proxy terminated" << std::endl;
	}
	rpc_frontend_.close();
	rpc_backend_.close();
	rpc_control_.close();
}

void zqrpc::RpcServer::ProxyStart()
{
	zmq::socket_t* frontend_ = &rpc_frontend_.socket_;
	zmq::socket_t* backend_ = &rpc_backend_.socket_;
	zmq::socket_t* control_ = &rpc_control_.socket_;
	return zmq::proxy_steerable(
		ZQRPC_VOIDCAST(*frontend_),
		ZQRPC_VOIDCAST(*backend_),
		ZQRPC_NULL,
		ZQRPC_VOIDCAST(*control_)
	);
}


void zqrpc::RpcServer::ProxyStop()
{
		ZSocket rpc_c_(context_,ZMQ_DEALER,"TERMIN");
		rpc_c_.connect(use_inproc_pcontrol.c_str());
		rpc_c_.SendString("TERMINATE");
		rpc_c_.disconnect(use_inproc_pcontrol.c_str());
		rpc_c_.close();
}

void zqrpc::RpcServer::WorkerStop(std::size_t nos)
{
		ZSocket rpc_c_(context_,ZMQ_DEALER,"WSTOP");
		rpc_c_.connect(use_inproc_workil.c_str());
		for (std::size_t i=0;i<nos;++i) {
			rpc_c_.SendString("TERMINATE");
		}
		rpc_c_.disconnect(use_inproc_workil.c_str());
		rpc_c_.close();
}

