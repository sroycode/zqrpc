#ifndef _ZQRPC_COMMON_HPP_
#define _ZQRPC_COMMON_HPP_
#include <string>
#include <exception>
#include <cstdint>
#include <cstring>
#include <boost/thread.hpp>
#include <arpa/inet.h>
#include <sys/time.h>
#include "zmq.hpp"
#include <glog/logging.h>


#define ZQRPC_INPROC_WORKER "inproc://zqrpc.workers"

namespace zqrpc {

namespace {
inline long mstime()
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (long) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
}

namespace {
enum ErrorCodeE {
    ZEC_SUCCESS                 = 0,
    ZEC_INVALIDHEADER           = 1,
    ZEC_NOSUCHSERVICE           = 2,
    ZEC_NOSUCHMETHOD            = 3,
    ZEC_METHODNOTIMPLEMENTED    = 4,
    ZEC_CONNECTIONERROR         = 5,
    ZEC_DATACORRUPTED           = 6,
    ZEC_CLIENTERROR             = 7,
    ZEC_UNKNOWNERROR            = 8
};
} // namespace blank

struct RetryException {};
struct TimeoutException {};

class ZError : public std::exception {
public:

	ZError(ErrorCodeE ecode, const char* emesg="") : ecode_(ecode), emesg_(emesg) {}

	virtual void reset(ErrorCodeE ecode, const char* emesg="") {
		ecode_=ecode;
		emesg_=emesg;
	}

	virtual const char* what() const throw () {
		return emesg_;
	}

	virtual ErrorCodeE code() const throw () {
		return ecode_;
	}

	virtual int icode() const throw () {
		return (int)ecode_;
	}

private:
	ErrorCodeE ecode_;
	const char* emesg_;
};
}

namespace zqrpc {
class ZSocket {

public:
	ZSocket(zmq::context_t* context, int type, const std::string& id)
		: socket_(*context,type) {
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
			DLOG(INFO) << std::string(static_cast<char*>(msg.data()), msg.size()) << std::endl;
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
			DLOG(INFO) << std::string(static_cast<char*>(msg.data()), msg.size()) << std::endl;
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
			if (!socket_.recv(&msg, ZMQ_NOBLOCK)) throw zqrpc::RetryException();
			frames.push_back(std::string(static_cast<char*>(msg.data()), msg.size()));
			more = msg.more();
		} while(more);

		return frames;
	}

	bool Poll(long timeout) {
		zmq::pollitem_t items[] = { { socket_, 0, ZMQ_POLLIN, 0 } };
		zmq::poll (&items[0], 1, timeout);
		return (items[0].revents & ZMQ_POLLIN);
		DLOG(INFO) << "Polling End" << std::endl;
	}

private:
	zmq::socket_t socket_;

	ZSocket(const ZSocket&);
	void operator=(const ZSocket&);

};
} // namespace

#endif
