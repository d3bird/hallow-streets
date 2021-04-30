#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "model.h"
#include "shader.h"
#include "time.h"
#include "camera.h"
#include "animation_manger.h"
#include "object_manger.h"

class player{
public:
	player(Camera* c, bool free);
	~player();

	void draw();
	void update();
	void init();

	void set_projection(glm::mat4 i) { projection = i; update_projection = true; }
	void set_cam(glm::mat4 i) { view = i; update_cam = true; }

	void set_time(timing* i) { Time = i; }
	void set_cam(Camera* c) { cam = c; }

	void set_height_map(double** h) { height_map = h; }

	void set_free_camera(bool i) { free_camera = i; }

	bool did_pos_change();
	int get_x_cell() { return x_cell; }
	int get_z_cell() { return z_cell; }
	int get_direction();
	bool did_dir_change();

	void projected_movement(Camera_Movement move, float time);
	void projected_movement2(Camera_Movement move, float time);//testing function
	void projected_movement3(Camera_Movement move, float time);//testing function
	void projected_movement4(Camera_Movement move, float time);//testing function

	void print_heights_around_player();

	void drop_a_trap();
	void pick_up_trap() { traps++; }
	void set_trap_number(int i) { traps = i; }
	void set_OBJM(object_manger* o) { OBJM = o; }
	void set_animation_manager(animation_manager* A) { AM = A; }

	void activate_item();

private:

	enum look_direction {
		NORTH = 0, NORTH_EAST = 1, NORTH_WEST = 2, WEST = 3, EAST = 4,
		SOUTH = 5, SOUTH_EAST = 6, SOUTH_WEST = 7, LOST =-1
	};
	
	std::string look_direction_to_string(look_direction i);

	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;
	
	bool free_camera;
	int dir;
	bool dir_change;
	double** height_map;//the height for the player at each point

	int traps;
	object_manger* OBJM;
	animation_manager* AM;
	Model* moon;
	Shader* space;
	Camera* cam;
	timing* Time;
	float* deltatime;
	glm::vec3* look;
	int x_cell;
	int z_cell;

	double player_height;
	double tolerence;

	bool change_cube(glm::vec3& pos, int dir);
	bool can_pass_with(double heigh, int i, int h);
	int determ_dirction_moving(glm::vec3& pos, glm::vec3& new_pos);

};

