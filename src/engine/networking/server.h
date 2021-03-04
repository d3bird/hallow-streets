#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <string>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

static unsigned int id_count = 0;

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message& msg) = 0;

    unsigned int user_id;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

struct chat_participant_data {
    unsigned int user_id;

};

//----------------------------------------------------------------------

class chat_room
{
public:
    void join(chat_participant_ptr participant);

    void leave(chat_participant_ptr participant);

    void deliver(const chat_message& msg);

private:
    std::set<chat_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
    : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
    chat_session(tcp::socket socket, chat_room& room)
        : socket_(std::move(socket)),
        room_(room)
    {
    }

    void start();

    void deliver(const chat_message& msg);

private:

    //if returns then pass on the messsage to other particpants 
    bool parse_message(const chat_message& msg, unsigned int user_id);

    void do_read_header();

    void do_read_body();

    void do_write();

    tcp::socket socket_;
    chat_room& room_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
    chat_server(boost::asio::io_context& io_context,
        const tcp::endpoint& endpoint)
        : acceptor_(io_context, endpoint)
    {

        send_message = new command;
        spwan_item = new command;
        spwan_item->com = SPAWN_ITEM;
        update_item = new command;
        update_item->com = UPDATE_ITEM;
        do_accept();
    }

    ~chat_server() {
        delete send_message;
        delete spwan_item;
        delete update_item;
    }

    void send_message_to_clients();

private:
    void do_accept();

    chat_message create_message(command* input);

    command* send_message;
    command* spwan_item;
    command* update_item;

    tcp::acceptor acceptor_;
    chat_room room_;
};