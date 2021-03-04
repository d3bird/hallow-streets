#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <string>
#include <queue>

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

    std::queue<command*>* things_to_do = new std::queue<command*>();
    std::queue<command*>* get_inputed_commands() { return things_to_do; }

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
        things_to_do = room.get_inputed_commands();;
    }

    void start();

    void deliver(const chat_message& msg);

    std::queue<command*>* get_inputed_commands_from_session() { return things_to_do; }

private:

    //if returns then pass on the messsage to other particpants 
    void parse_message(const chat_message& msg, unsigned int user_id);
    command* generate_command(std::string data[], chat_commands com);

    void do_read_header();

    void do_read_body();

    void do_write();
    std::queue<command*>* things_to_do;
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
        do_accept();
    }

    std::queue<command*>* get_inputed_commands_from_session() { return room_.get_inputed_commands(); }

    void send_message_to_clients(const chat_message& msg);

private:
    void do_accept();

    std::queue<command*>* things_to_do;

    tcp::acceptor acceptor_;
    chat_room room_;
};