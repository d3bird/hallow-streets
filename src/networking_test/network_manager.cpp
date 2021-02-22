#include "network_manager.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

network_manager::network_manager() {
	server = NULL;
	Client = NULL;
}

network_manager::~network_manager(){
	if (server != NULL) {
		delete server;
	}
	if (Client != NULL) {
		delete Client;
	}
}

void network_manager::init() {
	std::cout << "creating network_manager" << std::endl;
	int port = 1234;
	std::string ip_adress = "127.0.0.1";
	std::cout << "testing to see if a server is up on ip: "<< ip_adress <<" port: " << port<< std::endl;

	if (!port_in_use(port)) {
		try
		{
			boost::asio::io_context io_context;
			server = new tcp_server(io_context);
			io_context.run();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	else {
		std::cout << "the port was already in use, starting up client" << std::endl;
		Client = new client(ip_adress, port);
		Client->connect();
	}
}

bool network_manager::port_in_use(unsigned short port) {
	using namespace boost::asio;
	using ip::tcp;

	io_service svc;
	tcp::acceptor a(svc);

	boost::system::error_code ec;
	a.open(tcp::v4(), ec) || a.bind({ tcp::v4(), port }, ec);

	return ec == error::address_in_use;
}