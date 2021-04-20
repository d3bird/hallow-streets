#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <random>
#include <stdlib.h>     /* srand, rand */

#include "model.h"
#include "shader.h"
#include "time.h"

/*
* the city_square is used to define sections of the city
* since the sidewalk is only on one side of the road the layout has to specify which road to place

*/

//what kind of cell to generate
enum city_square {
	reserverd = -1, cube = 1, open = 2, wall = 3, wall_d = 4, wall_c = 5, chicken_pen = 14,
	road_top = 0, road_bot = 6, road_left = 7, road_right = 8, road_turn = 9, road_open = 10, road_curve = 11, road_inside_curve = 12, small_road = 13,
	wall_loading = 14, roof_place = 15, outside = 16, inside = 17
};  //large roads

//what kind of district
enum district_type {industry, town_center, docks};

//what kind of building
enum building_type { workshop, shop, residental, blanks };


struct rail_section {
	glm::vec3 loc;
	glm::vec3 rot;
	float angle;
	int type =0;
};

struct item_gen_info {
	city_square item;
	int expanded_layout_value;
	bool on_wall;
	glm::vec3 loc;
	glm::vec3 rot;
	float angle;
	bool roof = false;
	int roof_value;
	int floor = 1;
	int total_floors = 1;
	int y_cube_offset = 0;
	int x_cube_offset = 0;
	int z_cube_offset = 0;
};

struct cell_data {
	city_square type;
	int x_loc;
	int y_loc;
	int** expanded_layout_info = NULL;
	std::vector< item_gen_info*> items_on_ground;
	std::vector< item_gen_info*> items_on_wall;
};

struct building {
	building_type build_type;
	district_type dis_type;

	std::vector<cell_data*> cell_info;
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

	bool has_generate_cell_info() { return generate_cell_info; }
	cell_data** get_cell_info() { return layout_cells; }

	double** get_height_map();

	bool* get_pas_key();
	int get_unq_obj_cnt() { return obj_count; }
	//misc function
	void print_layout();
	void print_expanded_layout();
	void print_height_map();

private:

	void create_city_block(int x1, int y1, int x2, int y2);

	void create_road(int x, int z, int  direct, int mid, int dir_change, bool debug = false);
	void create_buildings(int i_start, int h_start, int i_max, int h_max);

	void create_chicken_pen(int i_start, int h_start, int i_max, int h_max);

	void converte_expanded_to_height(int input, int i, int h);

	void create_height_map();
	bool is_road(int i, int h, int road_type =-1);

	

	//creates the city layout using the layout
	void create_expanded_layout();

	void create_road_tile(int start_x, int start_y, int set);
	void create_wall_tile(int start_x, int start_y);
	void create_empty_tile(int start_x, int start_y);
	void create_chicken_tile(int start_x, int start_y, int type);

	void create_tile_from_cell(int i, int h);

	bool premade;
	void use_premade_map();

	void find_space_for_buildings(int x1, int y1, int x2, int y2);

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

	int chicken_pen_amount;
	int start_r_x;
	int start_r_y;
	int rail_amount;
	std::vector< rail_section*> rails;

	//cell data 

	struct building_build_data {
		int x_start = -1;
		int y_start = -1;
		int x_end = -1;
		int y_end = -1;
		int size = -1;
	};

	building* generate_building(building_build_data* buiding_data);
	bool check_buiding_data(building_build_data* buiding_data);

	std::vector< building_build_data*> build_data;
	std::vector< building*> buildings;
	bool generate_cell_info;
	cell_data** layout_cells;
	//building* generated_building;

	double** height_map;
	double def_height;
	int obj_count;
	bool* pass_key;
};

/* generation key for what each number means on the expanded layout
 * 1:  place cube
 * 2:  place wall
 * 3:  place wall_d
 * 4:  place wall_c no rotate
 * 5:  place light post
 * 6:  place road (sidewalk top)
 * 7:  place road (sidewalk bottom)
 * 8:  place road (sidewalk left)
 * 9:  place road (sidewalk right)
 * 10: place road/path with no sidewalk
 * 11: place light post 90 degree trun
 * 12: place light post 180 degree trun
 * 13: place light post 270 degree trun
 * 14: place wall_c 90 degree trun
 * 15: place wall_c 180 degree trun
 * 16: place wall_c 270 degree trun
 * 17: chicken
 * 18 chicken ground
 * 19 wall 0 degree trun
 * 20 wall 90 degree trun
 * 21 wall 180 degree trun
 * 22 wall 270 degree trun
 * 23
 * 24 wall_d 0 degree trun
 * 25 wall_d 90 degree trun
 * 26 wall_d 180 degree trun
 * 27 wall_d 270 degree trun
 * 
 * 28 WALL_LA_T 0 degree trun
 * 29 WALL_LA_T 90 degree trun
 * 30 WALL_LA_T 180 degree trun
 * 31 WALL_LA_T 270 degree trun
 * 
 * 32 FLOOR_LA_T 0 degree trun
 * 33 FLOOR_LA_T 90 degree trun
 * 34 FLOOR_LA_T 180 degree trun
 * 35 FLOOR_LA_T 270 degree trun
 * 
 * 36 WALL_CLOCK_ANG_T 0 degree trun
 * 37 WALL_CLOCK_ANG_T 90 degree trun
 * 38 WALL_CLOCK_ANG_T 180 degree trun
 * 39 WALL_CLOCK_ANG_T 270 degree trun
 * 
 * 40 LOADING_DOOR_T 0 degree trun
 * 41 LOADING_DOOR_T 90 degree trun
 * 42 LOADING_DOOR_T 180 degree trun
 * 43 LOADING_DOOR_T 270 degree trun
 * 
 * 
 * 50 wall blocker
*/