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

#define PI 3.14159265

class sky{
public:
	sky();
	~sky();

	void draw();
	void update();
	void init();
	void weather_init();

	void set_projection(glm::mat4 i) { projection = i; update_projection = true; }
	void set_cam(glm::mat4 i) { view = i; update_cam = true; }

	void set_width(unsigned int x, unsigned int y, unsigned int z) { x_width = x; center_y = y; z_width = z; }

	glm::vec3 get_light_loc() { return glm::vec3(x, y, z); }
	void set_time(timing* i) { Time = i; }

	void pause_time_at_noon();
	void unpause() { paused = false; }
private:
	bool toggled_day;
	bool toggled_night;
	bool paused;
	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;

	float center_x;
	float center_z;
	float center_y;
	float angle;
	float radius;
	float decrease;
	float angle_incr;
	float x;
	float z;
	float y;
	unsigned int x_width;
	unsigned int z_width;

	Model* moon;
	Shader* space;

	timing* Time;
	float* deltatime;

	//weather data
	Shader* rain;
	float* rain_points;
	unsigned int rain_VBO, rain_VAO;
	bool raining;
};

