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
#include <string>
#include <set>
#include <random>

#include "model.h"
#include "shader.h"
#include "time.h"
#include "city_gen.h"
#include "object_manger.h"

using namespace std;

//#define ROW 9 
//#define COL 10 

typedef pair<int, int> Pair;
typedef pair<double, pair<int, int>> pPair;

struct cell {
	int parent_i, parent_j;
	// f = g + h 
	double f, g, h;
};


struct map_tile {
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int buffer_loc;
	int type;
	bool blocked;
	int g_cost;//move cost to move to this square
};

struct rail_cart {

};

struct rail_track {

	std::vector<glm::vec3> travel_points;

	std::vector< rail_cart*> carts_ontrack;
};


class city
{
public:
	city();
	~city();

	void draw();
	void draw_basline();

	void update();

	void init(object_manger* OBJM);

	//path finding function
	void aStarSearch(Pair src, Pair dest);

	void tracePath(cell** cellDetails, Pair dest);
	void print_map();

	//misc functions

	//getters
	int get_x_width() { return x_width; }
	int get_y_width() { return 0; }
	int get_z_width() { return z_width; }
	int get_max_cubes() {
		if (city_info == NULL) {
			city_info = new city_gen();
			return city_info->get_max_cubes();
		}
		else {
			return city_info->get_max_cubes();
		}
	}

	//setters
	void set_projection(glm::mat4 i) { projection = i; }
	void set_cam(glm::mat4 i) { view = i; }
	void set_time(timing* i) { Time = i; }
	void set_shader(Shader* i) { cube_shader = i; }

private:

	bool objects_inited;
	bool objects_representations_craeted;

	//check function to makesure that everything was inited
	void check();

	//helper functions for pathfinding
	double calculateHValue(int row, int col, Pair dest);

	bool isDestination(int row, int col, Pair dest);
	bool isUnBlocked(int row, int col);
	bool isValid(int row, int col);

	glm::mat4 view;
	glm::mat4 projection;
	timing* Time;

	city_gen* city_info;
	city_square** layout;

	//rail carts
	item* rail_carts;

	unsigned int cube_amount;//the amount of cubes that there are to display


	//city debug information
	bool draw_wall;
	bool draw_wall_c;
	bool draw_path_cubes;
	bool draw_light_posts;
	bool draw_sidewalk;
	glm::mat4* cube_matrices;//contains all the cubes mats
	Shader* cube_shader;

	//path finding data
	map_tile** terrian_map;
	unsigned int x_width = 9;// ROW;//rows
	unsigned int z_width = 10;//COL;//collums

};

