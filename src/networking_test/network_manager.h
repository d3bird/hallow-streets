#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "server.h"
#include "client.h"

class network_manager {
public:
	network_manager();
	~network_manager();

	void init();

	void open_connetion();

private:

	
};
