#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include "zqrpc/ZSocket.hpp"


namespace zqrpc {
struct Worker {
	typedef std::vector<std::string> FramesT;
	zmq::context_t* context_;

	Worker(zmq::context_t* context) : context_(context) {}
	~Worker() {}
	void operator() () {
		zqrpc::ZSocket socket(context_,ZMQ_DEALER,boost::lexical_cast<std::string>(boost::this_thread::get_id()));
		socket.connect(ZQRPC_INPROC_WORKER);
		socket.SetLinger(0);
		typedef std::deque<std::string> FramesT;
		FramesT frames;
		while (1) {
			frames.clear();
			frames = socket.BlockingRecv<FramesT>();
			BOOST_FOREACH(std::string s, frames) {
				std::cerr << "## " << s.length() << " " << s << std::endl;
				std::cerr << "## ";
				BOOST_FOREACH(char c , s) { std::cerr << " " << (int)c ; }
				std::cerr << std::endl;
			}
			socket.Send<FramesT>(frames);
		}
	}
};
}


int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	zmq::context_t context;
	zqrpc::ZSocket rpc_frontend_(&context,ZMQ_ROUTER,"ROUTER");
	zqrpc::ZSocket rpc_backend_(&context,ZMQ_DEALER,"DEALER");
	boost::thread_group threads;
	rpc_frontend_.bind("tcp://127.0.0.1:9089");
	rpc_backend_.bind(ZQRPC_INPROC_WORKER);
	std::size_t noof_threads=2;
	for(std::size_t i = 0; i < noof_threads; ++i) {
		zqrpc::Worker w(&context);
		threads.create_thread<zqrpc::Worker>(w);
	}
	rpc_frontend_.ProxyTo( rpc_backend_);
	try {
	} catch (zqrpc::ZError& e) {
		DLOG(INFO) << "ZError THROWN" << std::endl;
	} catch (zqrpc::RetryException& e) {
		DLOG(INFO) << "Retry Exception THROWN" << std::endl;
	} catch (zmq::error_t& e) {
		DLOG(INFO) << "ZMQ ZERROR THROWN : " << e.what() << std::endl;
	}
	threads.join_all();
}
