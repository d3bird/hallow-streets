#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>


class keyboard_manger
{
public:
	keyboard_manger();
	~keyboard_manger();

	char* key_board_input(int key, int action);

private:

};

