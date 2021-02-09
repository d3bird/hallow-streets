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

/*
* the city_square is used to define sections of the city
* since the sidewalk is only on one side of the road the layout has to specify which road to place

*/

enum city_square {  cube =1, open=2, wall=3, wall_d=4, wall_c=5, chicken_pen =14,
	road_top= 0 ,road_bot =6, road_left = 7, road_right = 8, road_turn = 9, road_open = 10, road_curve = 11, road_inside_curve = 12, small_road =13
};  //large roads


struct rail_section {
	glm::vec3 loc;
	glm::vec3 rot;
	float angle;
	int type =0;
};

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

	std::vector< rail_section*> get_rails() { return rails; }

	//misc function
	void print_layout();
	void print_expanded_layout();

private:

	void create_city_block(int x1, int y1, int x2, int y2);

	void create_road(int x, int z, int  direct, int mid, int dir_change, bool debug = false);
	void create_buildings(int i_start, int h_start, int i_max, int h_max);

	void create_chicken_pen(int i_start, int h_start, int i_max, int h_max);


	bool is_road(int i, int h);

	//creates the city layout using the layout
	void create_expanded_layout();

	void create_road_tile(int start_x, int start_y, int set);
	void create_wall_tile(int start_x, int start_y);
	void create_empty_tile(int start_x, int start_y);
	void create_chicken_tile(int start_x, int start_y, int type);

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
	bool gen_last_light;//used for alternating light post generation

	timing* Time;
	float* deltatime;

	std::vector< rail_section*> rails;

};

