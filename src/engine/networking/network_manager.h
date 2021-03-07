#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "server.h"
#include "client.h"
#include "../object_manger.h"
#include "../animation_manger.h"
#include "../text_rendering.h"

class network_manager {
public:
	network_manager();
	~network_manager();

	void init();

	void update();

	void set_paused(bool new_value);

	//not currently working
	bool is_server() { return server; }

	bool is_server_on_this_port() { return port_in_use(port); }

	void send_message_txt(std::string in);

	void send_message_spawn_object(int item, glm::vec3 &loc, glm::vec3 &rot_angle, float angle);
	void spawn_in_ours_and_conned_to(int item, glm::vec3& loc, glm::vec3& rot_angle, float angle);

	void send_message_spawn_actor(int item, glm::vec3& loc, glm::vec3& rot_angle, float angle, int id);
	void send_message_update_actor(int id, glm::vec3& loc, glm::vec3& rot_angle, float angle);

	//getters
	std::queue<command*>* get_inputed_cmmand_list() {
		if (client != NULL) {
			return client->get_given_command_pointer();
		}
		else {
			return servers->front().get_inputed_commands_from_session();
		}
	}

	//setters
	void set_text_engine(text_engine* txt) {
		text_render = txt;
		if (text_render != NULL) {
			messages_to_send = text_render->get_messages_to_send();
		}
	}

	void set_command_input(){ commands_recived = get_inputed_cmmand_list(); }
	void set_OBJM(object_manger* i) { OBJM = i; }
	void set_AM(animation_manager* i) { AM = i; actors = AM->get_actor_list(); }

	//misc

	void lock() {
		client->lock();
	}

	void unlock() {
		client->unlock();
	}

	void update_commands_to_share_world();

private:

	
	bool port_in_use(unsigned short port);
	chat_message create_message(command* input);
	void processes_command(command* com);

	void spawn_object_from_command(command* com);
	void update_actor_from_command(command* com);
	void spawn_actor_from_command(command* com);

	void update_all_actors();

	command* send_message;
	command* spwan_item;
	command* update_item;
	command* spawn_actor;

	text_engine* text_render;
	object_manger* OBJM;
	animation_manager* AM;

	std::queue<command*>* commands_recived;

	std::queue<std::string>* messages_to_send;

	std::vector<actor*>* actors;

	bool server;

	int port;
	std::string ip_adress;

	boost::mutex mtx_;

	chat_client* client;
	std::list<chat_server>* servers;

	std::vector<command*> send_world_commands;
	std::vector<chat_message> send_world_messages;

};
