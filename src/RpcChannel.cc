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
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <deque>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <climits>

zqrpc::RpcChannel::RpcChannel(zmq::context_t* context, const char* url) :
	context_(context),
	socket_(context,ZMQ_DEALER,boost::lexical_cast<std::string>(boost::this_thread::get_id())),
	url_(url),
	nextreply_(0)
{
	socket_.connect(url);
}

zqrpc::RpcChannel::~RpcChannel()
{
	Close();
}

void zqrpc::RpcChannel::ZSendMethod(const google::protobuf::MethodDescriptor* method,
                                    zqrpc::ZController* controller,
                                    const google::protobuf::Message* request)
{
	try {
		std::string opcode=std::string(method->full_name());
		std::string buffer;
		if (! request->SerializeToString(&buffer) )
			throw zqrpc::ZError(ZEC_INVALIDHEADER);
		typedef std::vector<std::string> FramesT;
		std::string myid = boost::lexical_cast<std::string>(boost::this_thread::get_id());
		std::string nextid = boost::lexical_cast<std::string>(++nextreply_);
		FramesT frames;
		frames.push_back("");
		frames.push_back(nextid);
		frames.push_back(opcode);
		frames.push_back(buffer);
		bool status = socket_.Send<FramesT>(frames);
		if (!status) throw zmq::error_t();

		// OK
		controller->rcode_ = ZRC_WORKING;
		controller->reqno_ = replylist_.size();
		replylist_[nextreply_]=controller;
	} catch (zqrpc::ZError& e) {
		controller->zerror_.reset(ZEC_INVALIDHEADER);
		controller->rcode_ = ZRC_ERROR;
	} catch (zqrpc::RetryException& e) {
		controller->zerror_.reset(ZEC_CONNECTIONERROR);
		controller->rcode_ = ZRC_ERROR;
	} catch (zmq::error_t& e) {
		controller->zerror_.reset(ZEC_CONNECTIONERROR);
		controller->rcode_ = ZRC_ERROR;
	} catch (std::exception& e) {
		controller->zerror_.reset(ZEC_CLIENTERROR);
		controller->rcode_ = ZRC_ERROR;
	}
}

void zqrpc::RpcChannel::ZRecvMethod(ZController* controller,
                                    google::protobuf::Message* response,
                                    long timeout)
{
	// TODO: handle -1 , bad hack to make it 60 secs
	if (timeout<=0) timeout=60000;

	// The Loop below keeps polling and taking in data
	const long start = zqrpc::mstime();
	bool toget=false;
	typedef std::vector<std::string> FramesT;
	FramesT frames;
	do {
		// check if we need to poll
		// if there is toget we poll and get the data anyway for whosoever
		// if not toget we check if our controller has got its data , if so break
		if (!toget && controller->rcode_ != ZRC_WORKING) break;
		long elapsed = zqrpc::mstime() - start;
		// if time elapsed is more than timeout
		if (elapsed > timeout) break;
		if (!toget) toget = socket_.Poll(timeout-elapsed);
		// if toget is false we have spent all the wait time
		if (!toget) break;

		// we have action
		typedef std::vector<std::string> FramesT;
		try {
			frames = socket_.NonBlockingRecv<FramesT>();
		} catch (zqrpc::RetryException& e) {
			// no more data , set toget false and loop
			toget=false;
			continue;
		}
		// discard bad data , but toget remains true hence loop
		if (frames.size() !=4) continue;
		// BLANK ID ERRORCODE DATA
		// data looks ok
		std::size_t nextid = boost::lexical_cast<std::size_t>(frames[1]);
		// check if nextid is rogue , reject and loop
		if (nextid > nextreply_) continue;
		// get the associated controller pointer
		zqrpc::RpcChannel::ReplyListT::iterator it = replylist_.find(nextid);
		if (it==replylist_.end()) continue;
		zqrpc::ZController* othercon = it->second;
		int error = atoi(frames.at(2).c_str());
		if (error) {
			othercon->zerror_ = zqrpc::ZError(static_cast<zqrpc::ErrorCodeE>(error),frames.at(4).c_str());
			othercon->rcode_ = ZRC_ERROR;
		} else {
			othercon->payload_ = frames.at(3);
			othercon->rcode_ = ZRC_READY;
		}
	} while(true);

	if (controller->rcode_==ZRC_READY)
		response->ParseFromString(controller->payload_);
}

void zqrpc::RpcChannel::Close()
{
	socket_.disconnect(url_);
	socket_.close();
}
