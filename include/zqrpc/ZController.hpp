/**
 * @project zqrpc
 * @file include/zqrpc/ZController.hpp
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
 *  ZController.hpp : ZController class handles reply
 *
 */
#ifndef _ZQRPC_ZCONTROLLER_HPP_
#define _ZQRPC_ZCONTROLLER_HPP_

#define MAX_SIZET (size_t)-1

#include "ZError.hpp"
#include "ZmqUtils.hpp"

namespace zqrpc {

namespace {
enum RespCodeE {
    ZRC_UNSET                   = 0,
    ZRC_WORKING                 = 1,
    ZRC_READY                   = 2,
    ZRC_ERROR                   = 3,
    ZRC_TIMEDOUT                = 4
};
} // namespace blank

struct ZController {
	std::size_t reqno_;
	RespCodeE rcode_;
	ZError zerror_;
	std::string payload_;

	ZController() : reqno_(MAX_SIZET), rcode_(ZRC_UNSET), zerror_(ZEC_SUCCESS) {}
	virtual ~ZController() {}

	virtual bool ok() {
		return (rcode_==ZRC_READY);
	}

};
}
#endif
