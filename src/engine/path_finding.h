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

using namespace std;

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

class path_finding{
public:
	path_finding();
	~path_finding();

	std::vector<glm::vec3>* get_pathing(int loc_x, int loc_z,int dest_x, int dest_z);

	void aStarSearch(Pair src, Pair dest);

	void tracePath(cell** cellDetails, Pair dest);
	void print_map();

	void init();

	void set_x_width(int i) { x_width = i; }
	void set_z_width(int i) { z_width = i; }
	void set_cube_amount(int i) { cube_amount = i; }
	void set_expanded_layout(int** map);
	void set_key_for_passing_through(bool* key, int amt) { 
		key_pass_through_key = key; amnt_of_dif_obj = amt;	}

	map_tile** get_map_for_debug(){return terrian_map; }

private:

	bool can_pass(int object_id);

	double calculateHValue(int row, int col, Pair dest);

	bool isDestination(int row, int col, Pair dest);
	bool isUnBlocked(int row, int col);
	bool isValid(int row, int col);

	std::vector<glm::vec3> *generated_points;

	int** expanded_layout;
	bool* key_pass_through_key;
	int amnt_of_dif_obj;

	//path finding data
	map_tile** terrian_map;
	unsigned int x_width = 9;// ROW;//rows
	unsigned int z_width = 10;//COL;//collums
	int cube_amount;

};

