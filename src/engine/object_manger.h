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

#include "model.h"
#include "shader.h"
#include "time.h"

/*
* this class maganges the information about the workshops, items, and misc furniture 
*/

enum item_type {CUBE_T, SIDEWALK_T, LIGHT_POST_T, WALL_T, WALL_C_T, SIDESTREET_T, TABLE};
//the information on each unique object
struct item_info {
	unsigned int debug_id;
	unsigned int item_id;
	unsigned int buffer_loc;
	unsigned int amount;
	int x_m, y_m, z_m;// the location in the world
	float x, y, z;//the location on the screen
	float x_scale;
	float y_scale;
	float z_scale;
	std::string* item_name;
	item_type type;
	//zone_loc* zone_location;//needed for when an object leaves a zone
	int stack_size;
	int max_stack_size;
	bool stackable;
	bool max_stack() { return stack_size == max_stack_size; } 
	bool edible =false;

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

class object_manger{
public:

	object_manger();
	~object_manger();

	void draw();
	void update();
	void init();

	void update_item_matrix(update_pak* data);

	item_info* spawn_item(item_type type,int x, int z, glm::mat4 given_mat = glm::mat4(-1.0f));

	void merge_item_stacks(item_info* keep, item_info* rm);
	void split_merge_item_stacks(item_info* keep, item_info* rm);
	item_info* split_item_stacks(item_info* keep, int amount);

	item_info* get_open_table();
	void return_table(item_info*i);

	//getters and setters
	void set_projection(glm::mat4 i) { projection = i; update_projection = true; }
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

private:

	void increase_buffer_size();

	void create_cube_objects();
	void create_sidewalk_objects();
	void create_light_post_object();
	void create_wall_object();
	void create_wall_c_fire();
	void create_sidestreet_object();
	void create_table_object();

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

	std::vector< item*> items;
	std::vector<block_loc*>* blocked_spots;
	

	//bool draw vars
	bool draw_cubes;
	bool draw_wall;
	bool draw_wall_c;
	bool draw_sidewalk;
	bool draw_light_post;

};

