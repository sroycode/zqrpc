/**
 * @project zqrpc
 * @file include/zqrpc/RpcWorker.hh
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
 *  RpcWorker.hh : Worker Headers
 *
 */
#ifndef _ZQRPC_WORKER_HH_
#define _ZQRPC_WORKER_HH_

#include <map>
#include <vector>
#include "ZmqUtils.hpp"
#include "RpcMethod.hpp"

namespace zqrpc {

class RpcWorker {
	typedef std::map<std::string,RpcMethod*> RpcMethodMapT;
	typedef std::vector<std::string> RpcBindVecT;
public:
	RpcWorker(zmq::context_t* context, RpcMethodMapT rpc_method_map,
		const bool noreply, const char* suffix=ZQRPC_DEF_SUFFIX);
	~RpcWorker();
	void operator() ();
private:
	zmq::context_t* context_;
	RpcMethodMapT rpc_method_map_;
	const bool noreply_;
	const std::string use_inproc_worker;

};
}
#endif
