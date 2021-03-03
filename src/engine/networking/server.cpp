#include "server.h"

void chat_room::join(chat_participant_ptr participant)
{
    participant->user_id = id_count;
    id_count++;
    std::cout << "user " << participant->user_id << " has just joined" << std::endl;
    participants_.insert(participant);
    for (auto msg : recent_msgs_)
        participant->deliver(msg);
}

void chat_room::leave(chat_participant_ptr participant)
{
    std::cout << "user " << participant->user_id << " has just left" << std::endl;
    participants_.erase(participant);
}

void chat_room::deliver(const chat_message& msg)
{
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();

    for (auto participant : participants_)
        participant->deliver(msg);
}

//----------------------------------------------------------------------

void chat_session::start()
{
    room_.join(shared_from_this());
    do_read_header();
}

void chat_session::deliver(const chat_message& msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
        do_write();
    }
}

void chat_session::do_read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec && read_msg_.decode_header())
            {
                do_read_body();
            }
            else
            {
                room_.leave(shared_from_this());
            }
        });
}

void chat_session::do_read_body()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                std::cout << "revcived a message from " << self->user_id << std::endl; 
                room_.deliver(read_msg_);
                do_read_header();
            }
            else
            {
                room_.leave(shared_from_this());
            }
        });
}

void chat_session::do_write()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                write_msgs_.pop_front();
                if (!write_msgs_.empty())
                {
                    do_write();
                }
            }
            else
            {
                room_.leave(shared_from_this());
            }
        });
}

//----------------------------------------------------------------------

void chat_server::do_accept()
{
    std::cout << "do_accept from chat sever run" << std::endl;
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<chat_session>(std::move(socket), room_)->start();
            }

            do_accept();
        });
}

//this function will send a message to every person in the sever
void chat_server::send_message_to_clients() {
    //char* line;// [chat_message::max_body_length + 1] ;
    //std::string temp_line = "hello from the sever";
    //line = &temp_line[0];
    std::cout << "server sending a message" << std::endl;
    char line[] = "server send this";
    chat_message msg;
    msg.body_length((std::strlen(line)));
    std::memcpy(msg.body(), line, msg.body_length());
    msg.encode_header();

    room_.deliver(msg);

}