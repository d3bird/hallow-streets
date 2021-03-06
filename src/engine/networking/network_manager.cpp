#include "network_manager.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

network_manager::network_manager() {
	server = false;
	port = 1234;
	ip_adress = "127.0.0.1";
	servers = NULL;
	client = NULL;

	send_message = new command;
	spwan_item = new command;
	spwan_item->com = SPAWN_ITEM;
	update_item = new command;
	update_item->com = UPDATE_ITEM;

	text_render = NULL;
	messages_to_send = NULL;
	OBJM = NULL;
}

network_manager::~network_manager(){
	delete send_message;
	delete spwan_item;
	delete update_item;
}

void network_manager::update() {
	if (commands_recived != NULL) {
		mtx_.lock();
		while (!commands_recived->empty()) {
			std::cout << "size: " << commands_recived->size() << std::endl;
			command* com = commands_recived->front();
			processes_command(com);
			commands_recived->pop();
			delete com;
		}

		if (messages_to_send != NULL && !messages_to_send->empty()) {
			while (!messages_to_send->empty()) {
				send_message_txt(messages_to_send->front());
				messages_to_send->pop();
			}
		}
		mtx_.unlock();
	}
}

void network_manager::processes_command(command* com) {
	std::cout << "processesing a given command" << std::endl;
	switch (com->com) {
	case MESSAGE:
		std::cout << "you got mail saying: "<<com->msg << std::endl;
		text_render->recive_message(com->msg);
		break;
	case SPAWN_ITEM:
			spawn_object_from_command(com);
		break;
	case UPDATE_ITEM:

		break;
	default:
		std::cout << "could not undersatnd command" << std::endl;
		break;
	}

}

void network_manager::send_message_spawn_object(int item, glm::vec3 &loc, glm::vec3 &rot_angle, float angle) {

	std::cout << "telling to spawn item at " << loc.x << " , "<<loc.y << " , "<<loc.z << " , " << std::endl;
	spwan_item->x = loc.x;
	spwan_item->y = loc.y;
	spwan_item->z = loc.z;

	spwan_item->rot_x = rot_angle.x;
	spwan_item->rot_y = rot_angle.y;
	spwan_item->rot_z = rot_angle.z;
	
	spwan_item->angle = angle;

	spwan_item->item = item;

	spwan_item->com = SPAWN_ITEM;
	chat_message msg = create_message(spwan_item);

	if (server) {
		if (servers != NULL) {
			servers->front().send_message_to_clients(msg);
		}
	}
	else {
		if (client != NULL) {
			client->write(msg);
		}
	}
}

void network_manager::spawn_object_from_command(command* com) {
	if (OBJM != NULL) {
		item_type type;
		switch (com->item)
		{
		case 0:
			type = CUBE_T;
			break;
		case 1:
			type = SIDEWALK_T;
			break;
		case  2:
			type = LIGHT_POST_T;
			break;
		case  3:
			type = WALL_T;
			break;
		case  4:
			type = WALL_C_T;
			break;
		case  5:
			type = SIDESTREET_T;
			break;
		case  6:
			type = SKYTRACK_S_T;
			break;
		case  7:
			type = SKYTRACK_C_T;
			break;
		case  8:
			type = CHICKEN_T;
			break;
		case  9:
			type = SKYTRACK_CART;
			break;
		case  10:
			type = CANNON_FRAME_T;
			break;
		case  11:
			type = CANNON_PLATFORM_T;
			break;
		case  12:
			type = CANNON_T;
			break;
		case  13:
			type = ZAP_TOWER_T;
			break;
		case  14:
			type = ZAP_SPHERE_T;
			break;
		case  15:
			type = CURSE_CHICKEN_T;
			break;
		default:
			std::cout << com->item<<" : not a understood item type" << std::endl;
			return;
			break;
		}

		glm::vec3 loc = glm::vec3(com->x, com->y, com->z);
		glm::vec3 rot_angle = glm::vec3(com->rot_x, com->rot_y, com->rot_z);
		glm::mat4 model = glm::mat4(1.0f);

		//std::cout << "telling to spawn item at " << loc.x << " , " << loc.y << " , " << loc.z << " , " << std::endl;


		model = glm::translate(model, loc);
		//model = glm::rotate(model, glm::radians(com->angle), rot_angle);

		item_info* item_data = OBJM->spawn_item(type, -1,-1,-1, model);
		item_data->x_rot = rot_angle.x;
		item_data->y_rot = rot_angle.y;
		item_data->z_rot = rot_angle.z;
		item_data->angle = com->angle;
		item_data->x = loc.x;
		item_data->y = loc.y;
		item_data->z = loc.z;
	}
	else {
		std::cout << "could not spawn item due to OBJM being null" << std::endl;
	}
}

void network_manager::spawn_in_ours_and_conned_to(int item, glm::vec3& loc, glm::vec3& rot_angle, float angle) {
	send_message_spawn_object(item, loc, rot_angle, angle);

	item_type type;
	switch (item)
	{
	case 0:
		type = CUBE_T;
		break;
	case 1:
		type = SIDEWALK_T;
		break;
	case  2:
		type = LIGHT_POST_T;
		break;
	case  3:
		type = WALL_T;
		break;
	case  4:
		type = WALL_C_T;
		break;
	case  5:
		type = SIDESTREET_T;
		break;
	case  6:
		type = SKYTRACK_S_T;
		break;
	case  7:
		type = SKYTRACK_C_T;
		break;
	case  8:
		type = CHICKEN_T;
		break;
	case  9:
		type = SKYTRACK_CART;
		break;
	case  10:
		type = CANNON_FRAME_T;
		break;
	case  11:
		type = CANNON_PLATFORM_T;
		break;
	case  12:
		type = CANNON_T;
		break;
	case  13:
		type = ZAP_TOWER_T;
		break;
	case  14:
		type = ZAP_SPHERE_T;
		break;
	case  15:
		type = CURSE_CHICKEN_T;
		break;
	default:
		std::cout << "not a understood item type" << std::endl;
		return;
		break;
	}


	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, loc);
	//model = glm::rotate(model, glm::radians(com->angle), rot_angle);

	item_info* item_data = OBJM->spawn_item(type, -1, -1, -1, model);
	item_data->x_rot = rot_angle.x;
	item_data->y_rot = rot_angle.y;
	item_data->z_rot = rot_angle.z;
	item_data->angle = angle;
	item_data->x = loc.x;
	item_data->y = loc.y;
	item_data->z = loc.z;

}

void network_manager::send_message_txt(std::string in) {
	std::cout << "sending a message" << std::endl;
	if (server) {
		if (servers != NULL) {
			send_message->msg = in;
			servers->front().send_message_to_clients(create_message(send_message));
		}
	}
	else {
		if (client != NULL) {
			send_message->msg = in;
			chat_message msg = create_message(send_message);
			client->write(msg);
		}
	}
}

chat_message network_manager::create_message(command* input) {
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
	std::cout << "created message length " << message.length() << std::endl;
	std::cout << "message: " << output.body() << std::endl;
	return output;
}

void network_manager::init() {
	std::cout << "creating network_manager" << std::endl;
//	int port = 1234;
	//std::string ip_adress = "127.0.0.1";
	std::cout << "testing to see if a server is up on ip: "<< ip_adress <<" port: " << port<< std::endl;

	if (!port_in_use(port)) {
		std::cout << "starting up server" << std::endl;
		server = true;
		try
		{

			boost::asio::io_context io_context;

			servers = new std::list<chat_server>();
			// for (int i = 1; i < argc; ++i)
			// {
			tcp::endpoint endpoint(tcp::v4(), port);
			servers->emplace_back(io_context, endpoint);
			//}

			io_context.run();
			//std::cout << "made it past the run"  << std::endl;
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}
	else {
		std::cout << "the port was already in use, starting up client" << std::endl;
		try
		{

			boost::asio::io_context io_context;

			tcp::resolver resolver(io_context);
			auto endpoints = resolver.resolve(tcp::endpoint(boost::asio::ip::address::from_string(ip_adress), port));
			client = new chat_client(io_context, endpoints);

			std::thread t([&io_context]() { io_context.run(); });

			char line[chat_message::max_body_length + 1];
			while (std::cin.getline(line, chat_message::max_body_length + 1))
			{
				chat_message msg;
				msg.body_length(std::strlen(line));
				std::memcpy(msg.body(), line, msg.body_length());
				msg.encode_header();
				client->write(msg);
			}

			client->close();
			t.join();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}
}

bool network_manager::port_in_use(unsigned short port) {
	using namespace boost::asio;
	using ip::tcp;

	io_service svc;
	tcp::acceptor a(svc);

	boost::system::error_code ec;
	a.open(tcp::v4(), ec) || a.bind({ tcp::v4(), port }, ec);

	return ec == error::address_in_use;
}