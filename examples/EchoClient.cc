/**
 * @project zqrpc
 * @file examples/EchoClient.cc
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
 *  EchoClient.cc : Echo Client
 *
 */

#include <iostream>
#include "zqrpc.hpp"
#include "echo.pb.h"

#include "EchoEndpoint.hh"

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	zmq::context_t* context = 0;

	try {
		context = new zmq::context_t(1);
		echo::EchoRequest request;
		zqrpc::RpcChannel rpc_channel(context,ECHO_ENDPOINT_PORT);
		echo::EchoService::Stub stub(&rpc_channel);
		echo::EchoResponse response;
		request.set_message("123456789012345678901234567890123456");
		stub.Echo1(NULL, &request, &response, NULL);
		std::cerr << "REQUEST: " << request.DebugString() << std::endl;
		std::cerr << "REPLY: " << response.DebugString() << std::endl;

		request.set_message("654321098765432109876543210987654321");
		stub.Echo2(NULL, &request, &response, NULL);
		std::cerr << "REQUEST: " << request.DebugString() << std::endl;
		std::cerr << "REPLY: " << response.DebugString() << std::endl;

	} catch (zmq::error_t& e) {
		std::cerr << "ZMQ EXCEPTION : " << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cerr << "STD EXCEPTION : " << e.what() << std::endl;
	} catch (...) {
		std::cerr << " UNTRAPPED EXCEPTION " << std::endl;
	}
	if (context) delete context;
	return 0;
}

