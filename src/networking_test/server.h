#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class tcp_connection
    : public boost::enable_shared_from_this<tcp_connection>
{
public:

    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_context& io_context) { return pointer(new tcp_connection(io_context)); }
    tcp::socket& socket() { return socket_; }

    void start();

private:

    std::string make_daytime_string();

    tcp_connection(boost::asio::io_context& io_context)
        : socket_(io_context)
    {
    }

    void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
    {
    }

    tcp::socket socket_;
    std::string message_;
};

class tcp_server
{
public:
    tcp_server(boost::asio::io_context& io_context): io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 1234)){
        std::cout << "starting up tcp server" << std::endl;
        start_accept();
    }

private:
    void start_accept();

    void handle_accept(tcp_connection::pointer new_connection,
        const boost::system::error_code& error);

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
};