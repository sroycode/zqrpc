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
#include <vector>
#include "zqrpc.hpp"
#include "echo.pb.h"
#include "echo.zqrpc.h"

#include "EchoEndpoint.hh"

struct Basket {
	zqrpc::ZController controller;
	echo::EchoRequest request;
	echo::EchoResponse response;
	bool use_one;
	long waitfor;
	Basket(const char* message,bool use_echo_one, long wait_time)
		: use_one(use_echo_one),waitfor(wait_time) {
		request.set_message(message);
	}
	virtual ~Basket() {}
};

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	zmq::context_t* context = 0;
	typedef std::vector<struct Basket> BasketVecT;

	try {
		context = new zmq::context_t(1);
		zqrpc::RpcChannel rpc_channel(context,ECHO_ENDPOINT_PORT);
		echo::EchoService::Stub stub(&rpc_channel);

		BasketVecT bask;
		// 0-1
		bask.push_back(Basket("The foolish race of mankind",false,-1));
		bask.push_back(Basket("Are swarming below in the night;",false,-1));

		// 2-3
		bask.push_back(Basket("They shriek and rage and quarrel-",true,-1));
		bask.push_back(Basket("And all of them are right.",true,-1));

		// Send All
		for (std::size_t i=0; i<bask.size(); ++i) {
			if (bask[i].use_one)
				stub.Echo1_Send(&bask[i].controller, &bask[i].request);
			else
				stub.Echo2_Send(&bask[i].controller, &bask[i].request);
			std::cerr << "Request " << i << ":" << bask[i].request.DebugString() << std::endl;
		}

		// Receive 2-3
		std::cerr << "RECEIVING 2-end" << std::endl;
		for (std::size_t i=2; i<bask.size(); ++i) {
			if (bask[i].use_one)
				stub.Echo1_Recv(&bask[i].controller, &bask[i].response,bask[i].waitfor);
			else
				stub.Echo2_Recv(&bask[i].controller, &bask[i].response,bask[i].waitfor);

			if (bask[i].controller.ok())
				std::cerr << "Response" << i << ":" << bask[i].response.DebugString() << std::endl;
			else
				std::cerr << "Error in Response " << i << std::endl;
		}

		// Receive 0-1
		std::cerr << "RECEIVING 0-1" << std::endl;
		for (std::size_t i=0; i<2; ++i) {
			if (bask[i].use_one)
				stub.Echo1_Recv(&bask[i].controller, &bask[i].response,bask[i].waitfor);
			else
				stub.Echo2_Recv(&bask[i].controller, &bask[i].response,bask[i].waitfor);

			if (bask[i].controller.ok())
				std::cerr << "Response" << i << ":" << bask[i].response.DebugString() << std::endl;
			else
				std::cerr << "Error in Response " << i << std::endl;
		}


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

