/**
 * @project zqrpc
 * @file src/RpcChannel.cc
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
 *  RpcChannel.cc : Channel Implementation
 *
 */
#include <google/protobuf/descriptor.h>
#include "zqrpc/RpcHeaders.hh"
#include "zqrpc/RpcChannel.hh"
#include <boost/lexical_cast.hpp>
#include <deque>
#include <cstdlib>

zqrpc::RpcChannel::RpcChannel(zmq::context_t* context, const char* url) :
	context_(context),
	socket_(context,ZMQ_REQ,boost::lexical_cast<std::string>(boost::this_thread::get_id())),
	url_(url)
{
	socket_.connect(url);
}

zqrpc::RpcChannel::~RpcChannel()
{
	Close();
}

void zqrpc::RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                                   google::protobuf::RpcController* controller,
                                   const google::protobuf::Message* request,
                                   google::protobuf::Message* response,
                                   google::protobuf::Closure* done)
{
	try {
		std::string opcode=std::string(method->full_name());
		std::string buffer;
		std::string whoami = boost::lexical_cast<std::string>(boost::this_thread::get_id() );
		request->SerializeToString(&buffer);
		typedef std::deque<std::string> FramesT;
		FramesT frames;
		frames.push_back("");
		frames.push_back(opcode);
		frames.push_back(buffer);
		bool status = socket_.Send(frames);
		// recv
		if (status)
			frames = socket_.BlockingRecv<FramesT>();
		if (frames.size() !=3)
			throw zqrpc::ZError(ZEC_INVALIDHEADER);
		// BLANK ERRORCODE DATA
		int error = atoi(frames.at(1).c_str());
		if (error) {
			// TODO: have to throw proper here SHREOS
			throw zqrpc::ZError(static_cast<zqrpc::ErrorCodeE>(error),frames.at(2).c_str());
		}
		response->ParseFromString(frames.at(2));
	} catch (zqrpc::ZError& e) {
		controller->SetFailed(std::string(e.what()));
	} catch (std::exception& e) {
		controller->SetFailed("Unknown Error");
	}
}

void zqrpc::RpcChannel::Close()
{
	socket_.disconnect(url_);
	socket_.close();
}
