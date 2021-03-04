#include "client.h"

void chat_client::write(const chat_message& msg)
{
    boost::asio::post(io_context_,
        [this, msg]()
        {
            bool write_in_progress = !write_msgs_.empty();
            write_msgs_.push_back(msg);
            if (!write_in_progress)
            {
                do_write();
            }
        });
}


void chat_client::do_connect(const tcp::resolver::results_type& endpoints)
{
    boost::asio::async_connect(socket_, endpoints,
        [this](boost::system::error_code ec, tcp::endpoint)
        {
            if (!ec)
            {
                do_read_header();
            }
        });
}

void chat_client::do_read_header()
{
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec && read_msg_.decode_header())
            {
                do_read_body();
            }
            else
            {
                socket_.close();
            }
        });
}

void chat_client::do_read_body()
{
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                parse_message(read_msg_,-1);
              //  std::cout.write(read_msg_.body(), read_msg_.body_length());
              //  std::cout << "\n";
                do_read_header();
            }
            else
            {
                socket_.close();
            }
        });
}

void chat_client::do_write()
{
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
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
                socket_.close();
            }
        });
}

void chat_client::parse_message(const chat_message& msg, unsigned int user_id) {
   // std::cout << "parsing message from user " << user_id << std::endl;
    const char* message = msg.body();

    std::string s = msg.body();
    std::string delimiter = "/";
    size_t pos = 0;
    std::string token[4];
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
            if (token_id >= 4) {
                break;
            }
        }

        token[3] = s;

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

command* chat_client::generate_command(std::string data[], chat_commands com) {
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
        switch (token_id) {
        case 0:
            x = std::stof(token[token_id]);
            break;
        case 1:
            y = std::stof(token[token_id]);
            break;
        case 2:
            z = std::stof(token[token_id]);
            break;
        }
        s.erase(0, pos + delimiter.length());
        token_id++;
        if (token_id >= 3) {
            break;
        }
    }
    z = x = std::stof(s);
    //std::cout <<s << std::endl;

    output->set_cords(x, y, z);

    token_id = 0;
    pos = 0;
    s = data[1];
    //std::cout << "grabbing rot data" << std::endl;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token[token_id] = s.substr(0, pos);
        //std::cout << token[token_id] << std::endl;
        switch (token_id) {
        case 0:
            x = std::stof(token[token_id]);
            break;
        case 1:
            y = std::stof(token[token_id]);
            break;
        case 2:
            z = std::stof(token[token_id]);
            break;
        }
        s.erase(0, pos + delimiter.length());
        token_id++;
        if (token_id >= 3) {
            break;
        }
    }
    z = std::stof(s);
    //  std::cout << s << std::endl;


  //  std::cout << "grabbing angle data" << std::endl;
      //  std::cout << data[2] << std::endl;

    output->set_rot(x, y, z, std::stof(data[2]));

    if (com == SPAWN_ITEM) {
        //  std::cout << "grabbing item code" << std::endl;
        output->item = std::stoi(data[3]);
    }
    else {
        //   std::cout << "grabbing actor id" << std::endl;
        output->actor_id = std::stoi(data[3]);
    }
    // std::cout << std::stoi(data[3]) << std::endl;



    return output;
}