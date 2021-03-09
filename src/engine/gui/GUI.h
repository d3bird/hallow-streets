#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "../shader.h"
#include "../time.h"
#include "../text_rendering.h"

class GUI{
public:
	GUI();
	~GUI();

	void draw();
	void update();

	void init();

	void set_projection(glm::mat4 i) { projection = i; }
	void set_cam(glm::mat4 i) { view = i; }
	void set_time(timing* i) { Time = i; }

	void set_text_engine(text_engine* txt) { text_render = txt; }
	void set_online(bool i) { online = i; }

private:

	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cam_pos;

	timing* Time;
	float* deltatime;

	bool online;
	bool server;
	text_engine* text_render;

	unsigned int VBO, VAO;
	Shader* gui_window;
};

