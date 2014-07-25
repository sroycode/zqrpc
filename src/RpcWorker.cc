/**
 * @project zqrpc
 * @file src/RpcWorker.cc
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
 *  RpcWorker.cc : Worker Implamentation
 *
 */
#include <deque>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include "zqrpc/RpcHeaders.hh"
#include "zqrpc/RpcWorker.hh"


zqrpc::RpcWorker::RpcWorker(zmq::context_t* context, RpcMethodMapT rpc_method_map)
	: context_(context),
	  rpc_method_map_(rpc_method_map)
{}

zqrpc::RpcWorker::~RpcWorker() {}

void zqrpc::RpcWorker::operator() ()
{
	zqrpc::ZSocket socket(context_,ZMQ_DEALER,boost::lexical_cast<std::string>(boost::this_thread::get_id()));
	socket.connect(ZQRPC_INPROC_WORKER);
	socket.SetLinger(0);
	google::protobuf::Message* request = NULL;
	google::protobuf::Message* response = NULL;
	typedef std::vector<std::string> FramesT;
	FramesT frames;
	while (1) {
		try {
			std::string nextid="99999";
			frames.clear();
			frames = socket.BlockingRecv<FramesT>();
			try {
				if (frames.size() !=5)
					continue;
				nextid = frames.at(2);
				std::string opcode = frames.at(3);
				std::string buffer = frames.at(4);
				frames.pop_back();
				frames.pop_back();
				DLOG(INFO) << "nextid->" << nextid << std::endl;
				DLOG(INFO) << "opcode->" << opcode << std::endl;
				DLOG(INFO) << "buffer->" << buffer << std::endl;
				RpcMethodMapT::const_iterator iter = rpc_method_map_.find( opcode );
				if (iter == rpc_method_map_.end())
					throw zqrpc::ZError(ZEC_NOSUCHMETHOD);

				RpcMethod *rpc_method = iter->second;
				const google::protobuf::MethodDescriptor *method = rpc_method->method_;
				google::protobuf::Message *request = rpc_method->request_->New();
				google::protobuf::Message *response = rpc_method->response_->New();
				if (! request->ParseFromString( buffer ) )
					throw zqrpc::ZError(ZEC_DATACORRUPTED);
				rpc_method->service_->CallMethod(method,request, response);
				if (!response->SerializeToString(&buffer) )
					throw zqrpc::ZError(ZEC_DATACORRUPTED);
				// send response message back
				DLOG(INFO) << boost::this_thread::get_id() << " :outbuf: " << buffer << std::endl;
				// BLANK ID ERRORCODE DATA
				frames.push_back("0"); // Error Code 0 = ZEC_SUCCESS
				frames.push_back(buffer);
				socket.Send<FramesT>(frames);
			} catch(const zqrpc::ZError& e) {
				// BLANK ID ERRORCODE DATA
				frames.push_back(boost::lexical_cast<std::string>(e.icode()));
				frames.push_back(std::string(e.what()));
				socket.Send<FramesT>(frames);
			}
		} catch(const zmq::error_t& e) {
			DLOG(INFO) << boost::this_thread::get_id() << " :error: " << e.what() << std::endl;
		} catch(const zqrpc::RetryException& e) {
			DLOG(INFO) << boost::this_thread::get_id() << " :error: retry " << std::endl;
		}
	}
	delete request;
	delete response;
	socket.disconnect(ZQRPC_INPROC_WORKER);
}
