#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

int main() {
  boost::asio::io_service io_service;
  tcp::socket socket(io_service);

  socket.connect( tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234 ) );

  const string msg = "Hello from Client!\n";
  boost::system::error_code error;
  boost::asio::write( socket, boost::asio::buffer(msg), error );

  if( !error ) {
    cout << "Client sent hello message!" << endl;
  }
  else {
    cout << "send failed: " << error.message() << endl;
  }

  boost::asio::streambuf receive_buffer;
  boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);

  if( error && error != boost::asio::error::eof ) {
    cout << "receive failed: " << error.message() << endl;
  }
  else {
    const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
    cout << data << endl;
  }

  cout << "reached end of program" << std::endl;
  while (true);
  return 0;
}
