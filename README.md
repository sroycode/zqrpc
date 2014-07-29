# ZQRPC: Protocol Buffer RPC transport

The idea is to create a mechanism for sending multiple requests if parallel and getting their answers
when needed. In between, the client thread is free to do its own work.
It is not necessary to fetch results in the same order as they have been submitted.

Please have a look at the examples folder.

## Usage

### Server
1. Start the context with n=1 string(s) and set up the server
<pre>
	context = new zmq::context_t(1);
	zqrpc::RpcServer rpc_server(context);
</pre>

2. Set Listening endpoints.
<pre>
	rpc_server.EndPoint("tcp://*.9038");
	rpc_server.EndPoint("tcp://*.9039");
</pre>

3. Register the service of the handler implementations.
<pre>
	zqrpc::ServiceBase *service = new EchoServiceImpl();
	rpc_server.RegisterService(service);
</pre>

4. Start the server service with n=5 workers
<pre>
	rpc_server.Start(5);
</pre>

### Client

1. Start the context with n=1 string(s)
<pre>
	context = new zmq::context_t(1);
</pre>

2. Create a channel to connect to a tcp host/port OR ipc OR inproc
<pre>
	zqrpc::RpcChannel rpc_channel(context,"http://127.0.0.1:9038");
	echo::EchoService::Stub stub(&rpc_channel);
</pre>

3. We need a controller, request and reply
<pre>
	zqrpc::ZController controller;
	echo::EchoRequest request;
	echo::EchoResponse response;
</pre>

4. To have a single request
<pre>
	long timeout=100; // milliseconds
	stub.Echo1(&controller, &request,&response, timeout);
	if (controller.ok() ) // have got result 
	.....
</pre>

5. For several parallel requests send them together and receive the results as you need them.
Order of receive need not match that of send. The timeout holds for that request only, please
refer to the example provided for a timer implementation.

<pre>
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


## Compile

### Requirements

- a C++ compiler ( g++/clang++)
- cmake ( v2.8+ )
- ZeroMQ ( v3.2+ )
- Google Protocol Buffers ( v2.5 )
- Google Logging Framework - glog ( v0.3.3 )
- Boost ( v1.48+)

### To Compile

<pre>
mkdir build
cd build
cmake .. 
make
</pre>

### Run the example

1. On one terminal from the build directory run
<pre>
export GLOG_logtostderr=1
./example/EchoServer
</pre>

2. On another terminal from the build directory run
<pre>
export GLOG_logtostderr=1 
./example/EchocLient
</pre>



##Thank You

Kevin Sapper (sappo)
