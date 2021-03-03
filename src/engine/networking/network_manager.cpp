#include "network_manager.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

network_manager::network_manager() {
	server = false;
	port = 1234;
	ip_adress = "127.0.0.1";
	servers = NULL;
	client = NULL;
}

network_manager::~network_manager(){

}

void network_manager::send_message() {
	std::cout << "sending a message" << std::endl;
	if (server) {
		if (servers != NULL) {
			servers->front().send_message_to_clients();
		}
	}
	else {
		if (client != NULL) {
			std::cout << "client sending a message" << std::endl;
			char line[] = "client send this";
			chat_message msg;
			msg.body_length((std::strlen(line)));
			std::memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			client->write(msg);
		}
	}
}


void network_manager::init() {
	std::cout << "creating network_manager" << std::endl;
//	int port = 1234;
	//std::string ip_adress = "127.0.0.1";
	std::cout << "testing to see if a server is up on ip: "<< ip_adress <<" port: " << port<< std::endl;

	if (!port_in_use(port)) {
		std::cout << "starting up server" << std::endl;
		server = true;
		try
		{

			boost::asio::io_context io_context;

			servers = new std::list<chat_server>();
			// for (int i = 1; i < argc; ++i)
			// {
			tcp::endpoint endpoint(tcp::v4(), port);
			servers->emplace_back(io_context, endpoint);
			//}

			io_context.run();
			//std::cout << "made it past the run"  << std::endl;
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}
	else {
		std::cout << "the port was already in use, starting up client" << std::endl;
		try
		{

			boost::asio::io_context io_context;

			tcp::resolver resolver(io_context);
			auto endpoints = resolver.resolve(tcp::endpoint(boost::asio::ip::address::from_string(ip_adress), port));
			client = new chat_client(io_context, endpoints);

			std::thread t([&io_context]() { io_context.run(); });

			char line[chat_message::max_body_length + 1];
			while (std::cin.getline(line, chat_message::max_body_length + 1))
			{
				chat_message msg;
				msg.body_length(std::strlen(line));
				std::memcpy(msg.body(), line, msg.body_length());
				msg.encode_header();
				client->write(msg);
			}

			client->close();
			t.join();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
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