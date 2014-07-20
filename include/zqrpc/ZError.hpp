/**
 * @project zqrpc
 * @file include/zqrpc/ZError.hpp
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
 *  ZError.hpp : ZError class handles string to zmq_message conversion
 *
 */
#ifndef _ZQRPC_ZERROR_HPP_
#define _ZQRPC_ZERROR_HPP_
#include <string>
#include <exception>


namespace zqrpc {

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
#endif
