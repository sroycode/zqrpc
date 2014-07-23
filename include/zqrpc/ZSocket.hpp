/**
 * @project zqrpc
 * @file include/zqrpc/ZSocket.hpp
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
 *  ZSocket.hpp : Send and Recv
 *
 */
#ifndef _ZQRPC_ZSOCKET_HPP_
#define _ZQRPC_ZSOCKET_HPP_

#include <stdint.h>
#include <cstring>
#include <boost/thread.hpp>
#include "zmq.hpp"
#include "ZError.hpp"
#include "RpcHeaders.hh"

namespace zqrpc {
class ZSocket {

public:
	ZSocket(zmq::context_t* context, int type, const std::string& id)
		: socket_(*context,type)
	{
		if (id.length()>0)
			socket_.setsockopt(ZMQ_IDENTITY, id.c_str(), id.length());
	}

	~ZSocket() {
	}

	void connect(const char* url) {
		socket_.connect(url);
	}

	void disconnect(const char* url) {
		if ( socket_.connected() ) socket_.disconnect(url);
	}

	void bind(const char* url) {
		socket_.bind(url);
	}
	void unbind(const char* url) {
		socket_.unbind(url);
	}

	void close() {
		socket_.close();
	}

	void ProxyTo(ZSocket& zsock) {
		zmq::proxy(socket_,zsock.socket_,NULL);
	}

	void SetLinger(int linger = -1) {
		socket_.setsockopt(ZMQ_LINGER, &linger, sizeof(int));
	}

	template<typename T>
	bool Send(const T& frames) {
		std::size_t fleft= frames.size();
		if(!fleft) return true;
		for (typename T::const_iterator it = frames.begin(); it!=frames.end(); ++it,--fleft) {
			zmq::message_t msg((void*)it->c_str(),it->length(),NULL);
			// DLOG(INFO) << std::string(static_cast<char*>(msg.data()), msg.size()) << std::endl;
			if (! socket_.send(msg, (fleft==1) ? 0 : ZMQ_SNDMORE) )
				throw ZError(ZEC_CONNECTIONERROR,"cannot send data frame");
		}
		return true;
	}

	template<typename T>
	T BlockingRecv() {
		T frames;
		bool more=false;
		do {
			zmq::message_t msg(0);
			if (!socket_.recv(&msg, 0)) throw zqrpc::RetryException();
			frames.push_back(std::string(static_cast<char*>(msg.data()), msg.size()));
			// DLOG(INFO) << std::string(static_cast<char*>(msg.data()), msg.size()) << std::endl;
			more = msg.more();
		} while(more);

		return frames;
	}

	template<typename T>
	T NonBlockingRecv() {
		T frames;
		zmq::message_t msg(0);
		bool more=false;
		do {
			if (!socket_.recv(&msg, ZMQ_DONTWAIT)) throw zqrpc::RetryException();
			frames.push_back(std::string(static_cast<char*>(msg.data()), msg.size()));
			more = msg.more();
		} while(more);

		return frames;
	}

	bool Poll(long timeout) {
		zmq::pollitem_t items[] = { { socket_, 0, ZMQ_POLLIN, 0 } };
		zmq::poll (&items[0], 1, timeout);
		return (items[0].revents & ZMQ_POLLIN);
		// DLOG(INFO) << "Polling End" << std::endl;
	}

private:
	zmq::socket_t socket_;

	ZSocket(const ZSocket&);
	void operator=(const ZSocket&);

};
} // namespace

#endif
