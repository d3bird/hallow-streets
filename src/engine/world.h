#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <random>

#include "shader.h"
#include "time.h"
#include "city.h"
#include "sky.h"

class world{
public:
	world();
	~world();

	void draw();
	void update();

	void init();

	//misc functions

	//getters

	//setters
	void set_projection(glm::mat4 i) { projection = i; }
	void set_cam(glm::mat4 i) { view = i; }
	void set_time(timing* i) { Time = i; }

	void change_projection(glm::mat4 i);

private:

	void lighting_init();

	glm::mat4 view;
	glm::mat4 projection;
	timing* Time;

	Shader* lighting_in;

	city* City;
	sky* Sky;
	glm::vec3 lighting_loc;
};

