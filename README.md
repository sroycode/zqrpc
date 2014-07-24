ZQRPC: Protocol Buffer RPC transport
===================================


The idea is to create a mechanism for sending multiple requests if parallel and getting their answers
when needed. In between, the cclient thread is free to do its own work.
It is not necessary to fetch results in the same order as they have been submitted.

Please have a look at the examples folder.

This is work-in-progress.

Compile
=======

You will need

- a C++ compiler ( g++/clang++)
- cmake ( v2.8+ )
- ZeroMQ ( v3.2+ )
- Google Protocol Buffers ( v2.5 )
- Google Logging Framework - glog ( v0.3.3 )
- Boost ( v1.48+)

To Compile

<pre>
mkdir build
cd build
cmake .. 
make
</pre>

Thank You
=========
Kevin Sapper (sappo)
