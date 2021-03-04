#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "server.h"
#include "client.h"
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

	//setters
	void set_text_engine(text_engine* txt) { 
		text_render = txt; 
		if (text_render != NULL) {
			messages_to_send = text_render->get_messages_to_send();
		}
	}


	//getters
	std::queue<command*>* get_inputed_cmmand_list() {
		if (client != NULL) {
			return client->get_given_command_pointer();
		}
		else {
			return servers->front().get_inputed_commands_from_session();
		}
	}

	//misc

	void lock() {
		client->lock();
	}

	void unlock() {
		client->unlock();
	}

	void set_command_input(){ commands_recived = get_inputed_cmmand_list(); }

private:
	bool port_in_use(unsigned short port);
	chat_message create_message(command* input);
	void processes_command(command* com);
	command* send_message;
	command* spwan_item;
	command* update_item;

	text_engine* text_render;

	std::queue<command*>* commands_recived;

	std::queue<std::string>* messages_to_send;

	bool server;

	int port;
	std::string ip_adress;

	boost::mutex mtx_;

	chat_client* client;
	std::list<chat_server>* servers;
};
