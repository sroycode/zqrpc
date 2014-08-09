#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include "zqrpc/ZSocket.hpp"


int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	zmq::context_t context;
	zqrpc::ZSocket socket(&context,ZMQ_PUB,"PUBLISHER");
	socket.bind("tcp://127.0.0.1:9089");
	typedef std::vector<std::string> FramesT;
	FramesT frames;
	while (1) {
		try {
			frames.clear();
			frames.push_back("A");
			frames.push_back("This is to test 1");
			socket.Send<FramesT>(frames);
			usleep(100);
		} catch (zqrpc::ZError& e) {
			DLOG(INFO) << "ZError THROWN" << std::endl;
		} catch (zqrpc::RetryException& e) {
			DLOG(INFO) << "Retry Exception THROWN" << std::endl;
		} catch (zmq::error_t& e) {
			DLOG(INFO) << "ZMQ ZERROR THROWN : " << e.what() << std::endl;
		}
	}
}
