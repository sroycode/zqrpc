# ZQRPC: Protocol Buffer RPC transport

The idea is to create a mechanism for sending multiple requests if parallel and getting their answers
when needed. In between, the cclient thread is free to do its own work.
It is not necessary to fetch results in the same order as they have been submitted.

Please have a look at the examples folder.

Usage is thus:

## Single Request

<pre>
	// Set the context, mostly this will be shared across the app
	context = new zmq::context_t(1);
	// create a channel to connect to a tcp host/port OR ipc OR inproc
	zqrpc::RpcChannel rpc_channel(context,"http://127.0.0.1:9038");
	echo::EchoService::Stub stub(&rpc_channel);
	// we need a controller, request and reply
	zqrpc::ZController controller;
	echo::EchoRequest request;
	echo::EchoResponse response;
	// Single Request
	long timeout=100; // milliseconds
	stub.Echo1(&controller, &request,&response, timeout);
	if (controller.ok() ) // have got result 
	.....
</pre>

## Multiple Requests

For several parallel requests from the same program the syntax is thus:

<pre>
	// Multiple Requests
	// Send
	stub.Echo1_Send(&controller1, &request1);
	stub.Echo1_Send(&controller2, &request2);
	stub.Echo2_Send(&controller3, &request3);
	// Receive .. timeout holds for that particular request
	stub.Echo1_Recv(&controller1, &response1,timeout);
	if (controller1.ok() ) // have got result 
	...
	stub.Echo1_Recv(&controller2, &response2,timeout);
	if (controller2.ok() ) // have got result 
	...
	stub.Echo2_Recv(&controller3, &response3,timeout);
	if (controller3.ok() ) // have got result 
	...
	
</pre>


#Compile

## Requirements

- a C++ compiler ( g++/clang++)
- cmake ( v2.8+ )
- ZeroMQ ( v3.2+ )
- Google Protocol Buffers ( v2.5 )
- Google Logging Framework - glog ( v0.3.3 )
- Boost ( v1.48+)

## To Compile

<pre>
mkdir build
cd build
cmake .. 
make
</pre>

#Thank You

Kevin Sapper (sappo)
