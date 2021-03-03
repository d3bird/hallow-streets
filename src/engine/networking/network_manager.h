#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "server.h"
#include "client.h"


class network_manager {
public:
	network_manager();
	~network_manager();

	void init();

	void set_paused(bool new_value);

	bool is_server() { return server; }

	void send_message();

	void lock() {
		mtx_.lock();
	}

	void unlock() {
		mtx_.unlock();
	}

private:
	bool port_in_use(unsigned short port);
	
	bool server;

	int port;
	std::string ip_adress;

	boost::mutex mtx_;

	chat_client* client;
	std::list<chat_server>* servers;
};
