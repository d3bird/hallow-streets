#pragma once

#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class client{
public:
	client(std::string i, int por);
	~client();

	void connect();

private:
	std::string ip;
	int port;
	
};
