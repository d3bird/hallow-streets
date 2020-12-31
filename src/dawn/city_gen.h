#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include <stdlib.h>     /* srand, rand */
#include <time.h> 

#include "model.h"
#include "shader.h"
#include "time.h"


enum city_square { big_road= 0, small_road =1, open=2 };

class city_gen{
public:
	city_gen();
	~city_gen();

	void draw();
	void update();
	void init();

	//setters

	void set_projection(glm::mat4 i) { projection = i; update_projection = true; }
	void set_cam(glm::mat4 i) { view = i; update_cam = true; }

	void set_time(timing* i) { Time = i; }

	//getters
	int get_width() { return block_width; }
	int get_height() { return block_height; }

	city_square** get_layout() { return layout; }

	//misc function
	void print_layout();

private:

	void create_city_block(int x1, int y1, int x2, int y2);
	void create_road(int x, int z, int  direct, int mid, int dir_change);


	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;

	city_square** layout;

	//for one city block
	int block_width;//x
	int block_height;//y

	timing* Time;
	float* deltatime;

};

