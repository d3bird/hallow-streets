
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
       /* if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }*/

        boost::asio::io_context io_context;

      /*  tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
            resolver.resolve(argv[1], "daytime");*/

        tcp::socket socket(io_context);

       // boost::asio::connect(socket, endpoints);

        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));

        for (;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof) {
                std::cout << "Connection closed cleanly by peer" << std::endl;

                break; // Connection closed cleanly by peer.
            }
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            std::cout.write(buf.data(), len);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    std::cout << "end of program" << std::endl;
    while (true);
    return 0;
}
//
//int main() {
//  boost::asio::io_service io_service;
//  tcp::socket socket(io_service);
//
//  socket.connect( tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234 ) );
//
//  const string msg = "Hello from Client!\n";
//  boost::system::error_code error;
//  boost::asio::write( socket, boost::asio::buffer(msg), error );
//
//  if( !error ) {
//    cout << "Client sent hello message!" << endl;
//  }
//  else {
//    cout << "send failed: " << error.message() << endl;
//  }
//
//  boost::asio::streambuf receive_buffer;
//  boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
//
//  if( error && error != boost::asio::error::eof ) {
//    cout << "receive failed: " << error.message() << endl;
//  }
//  else {
//    const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
//    cout << data << endl;
//  }
//  while (true);
//  return 0;
//}
