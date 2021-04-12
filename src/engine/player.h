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

	bool did_pos_change();
	int get_x_cell() { return x_cell; }
	int get_z_cell() { return z_cell; }

private:

	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;
	
	bool free_camera;

	double** height_map;//the height for the player at each point

	Model* moon;
	Shader* space;
	Camera* cam;
	timing* Time;
	float* deltatime;
	
	int x_cell;
	int z_cell;

};

