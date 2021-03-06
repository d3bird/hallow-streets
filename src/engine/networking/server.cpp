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
                bool pass_on = false;
                parse_message(read_msg_, self->user_id);
                if (pass_on) {
                    room_.deliver(read_msg_);
                }
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


void chat_session::parse_message(const chat_message& msg, unsigned int user_id) {
    // std::cout << "parsing message from user " << user_id << std::endl;
    const char* message = msg.body();

    std::string s = msg.body();
    std::string delimiter = "/";
    size_t pos = 0;
    std::string token[6];
    int token_id = 0;

    bool spawn_item = false;

    chat_commands com = MESSAGE;

    float x, y, z;

    switch (message[0]) {
    case '0':
        //  std::cout << "chat message" << std::endl;
        com = MESSAGE;
        s = s.substr(0, msg.body_length());
        s.erase(0, 2);
        token[0] = s;
        //std::cout << "parsed message length = " << msg.body_length() << std::endl;
        things_to_do->push(generate_command(token, com));
        //  std::cout << "the message is: "<<s << std::endl;
        break;
    case '1':
        spawn_item = true;
    case '2':
        if (spawn_item) {
            //  std::cout << "update item message" << std::endl;
            com = SPAWN_ITEM;
        }
        else {
            //   std::cout << "update item message" << std::endl;
            com = UPDATE_ITEM;
        }

        s.erase(0, 2);
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token[token_id] = s.substr(0, pos);
            //std::cout << token[token_id] << std::endl;
            s.erase(0, pos + delimiter.length());
            token_id++;
            if (token_id >= 6) {
                break;
            }
        }

        token[5] = s;

 

        //lock();
        things_to_do->push(generate_command(token, com));
        //unlock();

        break;
    default:
        std::cout << "could not identify " << message[4] << std::endl;
    }

    std::cout.write(msg.body(), msg.body_length());
    std::cout << "\n";
}

command* chat_session::generate_command(std::string data[], chat_commands com) {
    command* output = new command;

    output->com = com;

    if (com == MESSAGE) {
        output->msg = data[0];
        //std::cout << "parsed message length " << output->msg.length() << std::endl;

        return output;
    }

    std::string delimiter = ",";
    size_t pos = 0;
    int token_id = 0;
    std::string token[3];

    float x, y, z;

    std::string s = data[0];
    // std::cout << "grabbing cord data" << std::endl;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token[token_id] = s.substr(0, pos);
        // std::cout << token[token_id] << std::endl;
        s.erase(0, pos + delimiter.length());
        token_id++;
        if (token_id >= 3) {
            break;
        }
    }
    x = std::stof(token[0]);
    y = std::stof(token[1]);
   // z = std::stof(token[2]);
    z = std::stof(s);
   // std::cout << "command to print at x: " << x << " " << y << " " << z << std::endl;
   // z = x = std::stof(s);
    //std::cout <<s << std::endl;

    output->set_cords(x, y, z);

    token_id = 0;
    pos = 0;
    s = data[1];
    //std::cout << "grabbing rot data" << std::endl;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token[token_id] = s.substr(0, pos);
        //std::cout << token[token_id] << std::endl;

        s.erase(0, pos + delimiter.length());
        token_id++;
        if (token_id >= 3) {
            break;
        }
    }
    x = std::stof(token[0]);
    y = std::stof(token[1]);
   // z = std::stof(token[2]);
    z = std::stof(s);
   // std::cout << "command to print at rot x: " << x << " " << y << " " << z<<std::endl;
        //  std::cout << s << std::endl;


  //  std::cout << "grabbing angle data" << std::endl;
      //  std::cout << data[2] << std::endl;

    output->set_rot(x, y, z, std::stof(data[2]));

    if (com == SPAWN_ITEM) {
        //  std::cout << "grabbing item code" << std::endl;
        
        output->item = std::stoi(data[3]);
        std::cout << "spawn_item_from message = " << data[3] << " interpided as " << output->item << std::endl;
    }
    else {
        //   std::cout << "grabbing actor id" << std::endl;
        output->actor_id = std::stoi(data[3]);
    }
    // std::cout << std::stoi(data[3]) << std::endl;

    return output;
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



void chat_server::send_message_to_clients(const chat_message& msg) {
    room_.deliver(msg);
}
