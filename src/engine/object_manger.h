#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>

#include "model.h"
#include "shader.h"
#include "time.h"

/*
* This class handlse all the rendering for each of the differnt objects
* each object has one draw call that will draw every instance of that object
* an object can be spawned at a loaction(x y z) or a given transform
* the created object will return a pointer to a object so if anything needs to be changed from the defualts it can be changed or add to a update list
*/


//uncoment for what demo you want the engine to build into
//#define DEMO1

enum item_type {
	CUBE_T, SIDEWALK_T, LIGHT_POST_T, WALL_T, WALL_C_T, SIDESTREET_T,
	SKYTRACK_S_T, SKYTRACK_C_T, CHICKEN_T, SKYTRACK_CART,
	CANNON_FRAME_T, CANNON_PLATFORM_T, CANNON_T,
	ZAP_TOWER_T, ZAP_SPHERE_T, CURSE_CHICKEN_T, WALL_D_T, WALL_LA_T, FLOOR_LA_T,
	WALL_CLOCK_ANG_T, LOADING_DOOR_T, LEAVER_BOX_T, LEAVER_T, TABLE_T, COMPUTER_T,
	SLANTED_ROOF_T, ROOF_FILL_T, GENERIC_FLOOR_T
};

//the information on each unique object
struct item_info {
	unsigned int debug_id;
	unsigned int item_id;
	unsigned int buffer_loc;
	unsigned int amount;
	float x_m, y_m, z_m;// the location in the world
	float x, y, z;//the location on the screen
	float x_scale=1;
	float y_scale=1;
	float z_scale=1;

	float x_rot =0;
	float y_rot =1;
	float z_rot=0;

	float angle =0;

	std::string* item_name;
	item_type type;

	int stack_size;
	int max_stack_size;
	bool stackable;
	bool max_stack() { return stack_size == max_stack_size; } 
	bool edible =false;
	int cell_num = -1;
};

//the data needed to render the objects in the world
struct item {
	Model* model;
	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	vector<item_info*> item_data;
	std::string* item_name;
	item_type type;
	bool updatemats =true;
	bool draw = true;
};

//update the matrix of a specfic item
struct update_pak {
	unsigned int item_id;
	unsigned int buffer_loc;
	float x,y,z;
	float x_scale;
	float y_scale;
	float z_scale;
};

//transfer object to tell the terrian what to block in bulk after the objects are created
struct block_loc {
	unsigned int x;
	unsigned int y;
	unsigned int z;
	block_loc(unsigned int x1, unsigned int z1) {
		x = x1;
		z = z1;
	}
};

//transfer object to tell the map where all the maps are located
struct item_loc {
	int x, y, z;
	item_info* object;
	item_loc(item_info* o, int x1, int y1, int z1) {
		object = o;
		x = x1;
		y = y1;
		z = z1;
	}
};

struct rending_cell {
	std::vector<item_info *> obj_in_cell;
	int start_x = -1;
	int start_z = -1;
	int end_x = -1;
	int end_z = -1;
};

struct direction_offsets {
	int x_start_off;
	bool x_s_set_to_player = false;
	int z_start_off;
	bool z_s_set_to_player = false;
	int x_end_off;
	bool x_e_set_to_player = false;
	int z_end_off;
	bool z_e_set_to_player = false;
};

class object_manger{
public:

	object_manger();
	~object_manger();

	void draw();
	void draw_optimised();

	void draw_cursed_ob();

	void init();

	void update_item_matrix(update_pak* data, glm::mat4 given_mat = glm::mat4(-1.0f));

	item_info* spawn_item(item_type type,int x, int y, int z, glm::mat4 given_mat = glm::mat4(-1.0f));

	void merge_item_stacks(item_info* keep, item_info* rm);
	void split_merge_item_stacks(item_info* keep, item_info* rm);
	item_info* split_item_stacks(item_info* keep, int amount);

	//getters and setters
	void set_projection(glm::mat4 i) { projection = i; update_projection = true; if (common != NULL) { common->setMat4("projection", i); } }
	void set_cam(glm::mat4 i) { view = i; update_cam = true; }
	void set_standered_shader(Shader* i) { common = i; }
	void set_max_cubes(int i) { max_cubes = i; };

	bool need_cam_updates() { return using_custom_shaders; }
	std::vector<block_loc*>* get_blocked_spots() { return blocked_spots; }

	void set_map_size(int i) { max_cubes = i; }

	//places the items on the world map after each object has been created
	std::vector< item_loc> place_items_init();

	void set_time(timing* i) { Time = i; }
	void delete_item_from_buffer(item_info* it);

	std::vector< item*>* get_all_item_info() { return &items; }

	std::string item_type_to_string(item_type i);

	//this is in terms of the expanded layput
	void set_block_size(int x_mi, int z_mi, int x_ma, int z_ma, int key_size) { 
		x_min = x_mi; z_min = z_mi; x_max = x_ma; z_max = z_ma; key = key_size;
	}

	void optimise_pipe_line();
	void set_veiw_distance(int i);
	void set_player_pos(int x, int z);
	void set_look_dir(int i) { look_dir = i; update_list = true; }
	void set_veiw_type(int i) { veiw_type = i; update_list = true;}
	void set_city_layout_cells(rending_cell** i, int x_w, int z_w);
	int get_veiw_distance() { return veiw_distance; }
	void set_veiw_width(int i) { width = i; update_list = true;}
	int get_veiw_width() {return width; }

	void create_height_map_debig(double** height_m, int height, int block_width, double def_height);

	void draw_height_map_degbug();
	//setting
	direction_offsets* get_east_off() { return east_off; }
	direction_offsets* get_west_off() { return west_off; }
	direction_offsets* get_north_off() { return north_off; }
	direction_offsets* get_south_off() { return south_off; }

#ifdef DEMO1
	void update_demo1();
	bool is_phase_two() { return phase_two; }
	glm::vec3 get_merge_point() { return merge_point; }
#endif // DEMO1

private:

	void aggrigate_items_to_draw();

	void create_cursed_object_buffer();

	void increase_buffer_size();

	void create_cube_objects();
	void create_sidewalk_objects();
	void create_light_post_object();
	void create_wall_object();
	void create_wall_c_fire();
	void create_sidestreet_object();
	void create_sky_track_s_object();
	void create_sky_track_c_object();
	void create_chicken_object();
	void create_sky_cart_object();

	void create_cannon_object();
	void create_zap_tower();

	void create_wall_door_object();
	void create_wall_loading_object();
	void create_wall_loading_floor_object();

	void create_wall_clock_object();
	void create_wall_loading_door_object();
	void create_leaver_object();
	void create_table_object();
	void create_computer_object();

	void create_slanted_roof_object();
	void create_roof_fill_object();
	void create_generic_floor_object();

	//timing
	timing* Time;
	float* deltatime;

	//common vars
	bool update_projection;
	bool update_cam;

	bool using_custom_shaders;

	unsigned int object_id;
	glm::mat4 view;
	glm::mat4 projection;
	Shader* common;

	int max_cubes;

	std::vector< item*> items;//every item
	std::vector<block_loc*>* blocked_spots;

	//bool draw vars
	bool draw_cubes;
	bool draw_wall;
	bool draw_wall_with_door;
	bool draw_wall_loading_area;
	bool draw_wall_c;
	bool draw_sidewalk;
	bool draw_light_post;
	bool draw_sideroads;
	bool draw_sky_rail_s;
	bool draw_sky_rail_c;
	bool draw_chicken;
	bool draw_cart;
	bool draw_cursed_chicken;
	bool draw_cannon;
	bool draw_zap_tower;
	bool draw_loading_area_floor;
	bool draw_wall_clock_ang;
	bool draw_leaver_object;
	bool draw_door_object;
	bool draw_loading_door;
	bool draw_table;
	bool draw_computer;
	bool draw_roofs;
	bool draw_generic_floor;

	//cursed objects
	Shader* cursed;
	float u_time;

	//optimised rendering
	void add_rending_cell_to_list(rending_cell cell);
	void clear_optimised_items();

	rending_cell** city_layout_cells;
	int city_x_width;
	int city_z_width;
	std::vector< item*> optimised_items;//items based on where the player is
	int x_min;
	int z_min; 
	int x_max;
	int z_max;
	int number_of_cells;
	int key;
	//what cell the player is in
	int player_x; 
	int player_z;

	bool update_list;
	int veiw_distance;
	int veiw_type =1;
	int look_dir;
	int width;
	direction_offsets* east_off;
	direction_offsets* west_off;
	direction_offsets* north_off;
	direction_offsets* south_off;


	item* height_map;

	//1 is square around the player
	//2 is 180 in front of the player
	//demo1 vars
#ifdef DEMO1
	float angle = 90.0f;
	float angle2 = 0.0f;

	float angle_incr;
	float move_incr;
	float shirnk_incr;
	glm::vec3 scale;
	
	float timmer;

	bool float_cubes;
	bool float_wall;
	bool float_wall_c;
	bool float_sidewalk;
	bool float_light_post;
	bool float_sideroads;
	bool float_rail;
	bool phase_two;
	bool converge;
	bool setpoints;
	bool orbit;
	glm::vec3 merge_point;
	std::vector<int> floaters;
#endif // DEMO1
};

