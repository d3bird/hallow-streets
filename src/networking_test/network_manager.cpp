#include "network_manager.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

network_manager::network_manager() {


}

network_manager::~network_manager(){

}

void network_manager::init() {

}

string read_(tcp::socket& socket) {
	boost::asio::streambuf buf;
	boost::asio::read_until(socket, buf, "\n");

	string data = boost::asio::buffer_cast<const char*>(buf.data());
	return data;
}


void send_(tcp::socket& socket, const string& message) {
	const string msg = message + "\n";
	boost::asio::write(socket, boost::asio::buffer(message));
}

void network_manager::open_connetion() {

}