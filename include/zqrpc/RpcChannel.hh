/**
 * @project zqrpc
 * @file include/zqrpc/RpcChannel.hh
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
 *  RpcChannel.hh : RpcChannel Headers for client
 *
 */
#ifndef _ZQRPC_CHANNEL_HH_
#define _ZQRPC_CHANNEL_HH_
#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <string>

#include <map>
#include <vector>
#include "ZmqUtils.hpp"
#include "ZController.hpp"

namespace zqrpc {
class RpcChannel {
public:
	typedef std::map<std::size_t,ZController*> ReplyListT;
	RpcChannel(zmq::context_t* context, const char* url);
	virtual ~RpcChannel();
	virtual void ZSendMethod(const google::protobuf::MethodDescriptor* method,
	                         ZController* controller,
	                         const google::protobuf::Message* request);
	virtual void ZRecvMethod(ZController* controller,
	                         google::protobuf::Message* response,
	                         long timeout);

	void Close();
protected:
	zmq::context_t* context_;
	ZSocket socket_;
	const char* url_;
	ReplyListT replylist_;
	std::size_t nextreply_;
};
}
#endif
