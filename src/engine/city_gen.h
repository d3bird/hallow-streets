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


enum city_square { big_road= 0, small_road =1, open=2, wall=3, wall_d=4, wall_c=5 };

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
	int get_max_cubes() { return max_cubes; }

	city_square** get_layout() { return layout; }
	int** get_expanded_layout() { return layout_e; }

	int get_expandion_key() { return key; }

	//misc function
	void print_layout();
	void print_expanded_layout();

private:

	void create_city_block(int x1, int y1, int x2, int y2);
	void create_road(int x, int z, int  direct, int mid, int dir_change);
	void create_buildings();

	//creates the city layout using the layout
	void create_expanded_layout();

	void create_road_tile(int start_x, int start_y);
	void create_wall_tile(int start_x, int start_y);
	void create_empty_tile(int start_x, int start_y);

	bool premade;
	void use_premade_map();


	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;

	city_square** layout;
	int key;
	int** layout_e;

	//for one city block
	int block_width;//x
	int block_height;//y

	int max_cubes;

	timing* Time;
	float* deltatime;

};

