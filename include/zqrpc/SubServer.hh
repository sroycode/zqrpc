/**
 * @project zqrpc
 * @file include/zqrpc/SubServer.hh
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
 *  SubServer.hh : Server Headers
 *
 */
#ifndef _ZQRPC_SUBSERVER_HH_
#define _ZQRPC_SUBSERVER_HH_

#include <map>
#include <vector>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "ZmqUtils.hpp"
#include "RpcMethod.hpp"
#include "ServiceBase.hpp"

namespace zqrpc {

class SubServer {
	typedef std::map<std::string,RpcMethod*> RpcMethodMapT;
	typedef std::vector<std::string> RpcBindVecT;
	typedef std::vector<boost::thread*> ThreadPtrVecT;
public:
	SubServer(zmq::context_t* context, const char* suffix=ZQRPC_DEF_SUFFIX);
	~SubServer();
	// add endpoints
	void EndPoint(const char* url);
	// start
	void Start(std::size_t noof_threads);
	// register a service
	void RegisterService(zqrpc::ServiceBase* service);
	// remove service
	void RemoveService(zqrpc::ServiceBase* service);
	// close
	void Close();

protected:
	
	zmq::context_t* context_;
	const char* suffix_;
	const std::string use_inproc_workil;
	const std::string use_inproc_worker;
	const std::string use_inproc_pcontrol;
	ZSocket rpc_frontend_;
	ZSocket rpc_backend_;
	ZSocket rpc_control_;
	RpcMethodMapT rpc_method_map_;
	RpcBindVecT rpc_bind_vec_;
	ThreadPtrVecT threads_;
	bool started_;

	void ProxyStart();
	void ProxyStop();
	void WorkerStop(std::size_t nos);
};
}
#endif
