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
                bool pass_on = parse_message(read_msg_, self->user_id);
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

bool chat_session::parse_message(const chat_message& msg, unsigned int user_id) {
    std::cout << "parsing message from user " << user_id << std::endl;
    bool pass_on = true;
    const char* message = msg.body();

    std::string s = msg.body();
    std::string delimiter = "/";
    size_t pos = 0;
    std::string token[4];
    int token_id = 0;
    switch (message[0]) {
    case '0':
        std::cout << "chat message" << std::endl;
        break;
    case '1':
    case '2':

        while ((pos = s.find(delimiter)) != std::string::npos) {
            token[token_id] = s.substr(0, pos);
            std::cout << token[token_id] << std::endl;
            s.erase(0, pos + delimiter.length());
            token_id++;
            if (token_id >= 4) {
                break;
            }
        }

        break;
    default:
        std::cout << "could not identify " << message[4] << std::endl;
    }

    std::cout << "update item message" << std::endl;
    std::cout << "spawn item message" << std::endl;
    std::cout.write(msg.body(), msg.body_length());
    std::cout << "\n";
    return pass_on;
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

    spwan_item->set_cords(1.1, 2.7, 43.123213);
    spwan_item->set_rot(0, 1, 0, 17);
    spwan_item->item = 3;

    update_item->set_cords(0, 7, 0);
    update_item->set_rot(1, 1, 0, 90);
    update_item->actor_id = 3;

    /*msg.body_length((std::strlen(line)));
    std::memcpy(msg.body(), line, msg.body_length());
    msg.encode_header();*/
   // chat_message msg = create_message(send_message);
    //room_.deliver(msg);
    chat_message msg = create_message(spwan_item);
    room_.deliver(msg);
 //   chat_message msg = create_message(update_item);
   // room_.deliver(msg);
}



chat_message chat_server::create_message(command* input) {
    chat_message output;

    std::string message = "0:";

    bool update_comm = false;;
    switch (input->com) {
    case SPAWN_ITEM://since they both use most of the same info
    case UPDATE_ITEM:
        update_comm = true;
        message += std::to_string(input->x);
        message += ",";
        message += std::to_string(input->y);
        message += ",";
        message += std::to_string(input->z);
        message += "/";

        message += std::to_string(input->rot_x);
        message += ",";
        message += std::to_string(input->rot_y);
        message += ",";
        message += std::to_string(input->rot_z);
        message += "/";
        message += std::to_string(input->angle);
        message += "/";

        if (input->com == SPAWN_ITEM) {
            message += std::to_string(input->item);
        }
        else {
            message += std::to_string(input->actor_id);
        }

        break;
    case MESSAGE:
    default:
        message += input->msg;
        break;
    }

    if (update_comm) {
        switch (input->com) {

        case SPAWN_ITEM:
            message[0] = '1';
            break;
        case UPDATE_ITEM:
            message[0] = '2';
            break;
        }
    }

    output.body_length(message.length());
    std::memcpy(output.body(), message.c_str(), output.body_length());
    output.encode_header();

    return output;
}
