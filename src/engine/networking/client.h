#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
    chat_client(boost::asio::io_context& io_context,
        const tcp::resolver::results_type& endpoints)
        : io_context_(io_context),
        socket_(io_context)
    {
        do_connect(endpoints);
    }

    void write(const chat_message& msg);

    void close()
    {
        boost::asio::post(io_context_, [this]() { socket_.close(); });
    }

private:
    void do_connect(const tcp::resolver::results_type& endpoints);

    void do_read_header();

    void do_read_body();

    void do_write();

private:
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};