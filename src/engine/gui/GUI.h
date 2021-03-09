#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "../shader.h"
#include "../time.h"
#include "../text_rendering.h"
#include "../object_manger.h"
#include "../animation_manger.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

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

	void set_OBJM(object_manger* i) { OBJM = i; }
	void set_AM(animation_manager* i) { AM = i;}

	void set_draw_debug_info(bool i) { draw_debug_info = i; }
	void toggle_draw_debug_info() { draw_debug_info = !draw_debug_info; }



private:

	void update_values();

	void draw_demo_window();
	void draw_example_from_class();

	void draw_server_window();
	bool draw_server_windows;

	void draw_model_window();
	bool draw_model_windows;
	bool spawn_item;
	bool edit_cell;
	bool show_item_stats;
	bool show_animation_stats;
	std::vector< item*>* item_info;
	std::vector<actor*>* actors;
	std::vector<routine*>* routines;
	void debug_info();
	//debug_info vars
	int tab;
	bool my_tool_active;
	//ImVec4 my_color;
	float* my_color;
	float* my_values;

	float* overal;
	float* effect;
	float* effects_3D;

	std::vector<float> last_frames;
	int number_of_values;
	float passed_time;
	float record_check_point;

	bool draw_debug_info;

	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cam_pos;

	timing* Time;
	float* deltatime;

	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;

	bool online;
	bool server;
	text_engine* text_render;
	object_manger* OBJM;
	animation_manager* AM;
	unsigned int VBO, VAO;
	Shader* gui_window;
};

