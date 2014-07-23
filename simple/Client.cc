#include <deque>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "zqrpc/ZSocket.hpp"

namespace zqrpc {

struct Client {
	typedef std::vector<std::string> FramesT;
	zmq::context_t* context_;
	ZSocket socket_;
	const char* url_;
	std::size_t nextreply_;

	Client(zmq::context_t* context, const char* url) :
		context_(context),
		socket_(context,ZMQ_DEALER,boost::lexical_cast<std::string>(boost::this_thread::get_id())),
		url_(url),
		nextreply_(0) {
		socket_.connect(url);
	}

	~Client() {
		socket_.disconnect(url_);
		socket_.close();
	}

	void Send(FramesT frames) {
		bool status = socket_.Send<FramesT>(frames);
		if (!status) throw zqrpc::RetryException();
	}


	FramesT Recv() {
		socket_.Poll(5000);
		return socket_.NonBlockingRecv<FramesT>();
	}

};
}

int main(int argc, char *argv[])
{
	if (argc==1) {
		std::cerr << "Usage : " << argv[0] << " params" << std::endl;
		std::cerr << "Params " << " b: blank, n: request id, i: identity, f: sample pb function , x: sample data" << std::endl;
		exit(1);
	}
	google::InitGoogleLogging(argv[0]);
	try {
		zmq::context_t context;
		zqrpc::Client client(&context,"tcp://127.0.0.1:9089");
		zqrpc::Client::FramesT frames;
		for (int i=1;i<argc;++i) {
			std::string item(argv[i]);
			if (item=="b") item = std::string("");
			else if (item=="n") item = boost::lexical_cast<std::string>(++client.nextreply_);
			else if (item=="i") item = boost::lexical_cast<std::string>(boost::this_thread::get_id());
			else if (item=="f") item = std::string("echo.EchoService.Echo1");
			else if (item=="x") {
				// boost::format fmt("\n$123456789012345678901234567890");
				boost::format fmt("\n$123456789012345678901");
				item = fmt.str();
			} else {
				/** do nothing */
			}
			frames.push_back(item);
		}
		BOOST_FOREACH(std::string s, frames) {
				std::cerr << "O## " << s.length() << " " << s << std::endl;
				std::cerr << "O## ";
				BOOST_FOREACH(char c , s) { std::cerr << " " << (int)c ; }
				std::cerr << std::endl;
		}
		client.Send(frames);
		frames = client.Recv();
		BOOST_FOREACH(std::string s, frames) {
				std::cerr << "I## " << s.length() << " " << s << std::endl;
				std::cerr << "I## ";
				BOOST_FOREACH(char c , s) { std::cerr << " " << (int)c ; }
				std::cerr << std::endl;
		}
		client.Send(frames);
	} catch (zqrpc::ZError& e) {
		DLOG(INFO) << "ZError THROWN" << std::endl;
	} catch (zqrpc::RetryException& e) {
		DLOG(INFO) << "Retry Exception THROWN" << std::endl;
	} catch (zmq::error_t& e) {
		DLOG(INFO) << "ZMQ ZERROR THROWN : " << e.what() << std::endl;
	}

}
