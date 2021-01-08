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

#include "model.h"
#include "shader.h"
#include "time.h"
#include "city_gen.h"


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

class city
{
public:
	city();
	~city();

	void draw();
	void update();

	void init();

	//path finding function
	void aStarSearch(Pair src, Pair dest);

	void tracePath(cell** cellDetails, Pair dest);
	void print_map();

	//misc functions

	//getters
	int get_x_width() { return x_width; }
	int get_y_width() { return 0; }
	int get_z_width() { return z_width; }

	//setters
	void set_projection(glm::mat4 i) { projection = i; }
	void set_cam(glm::mat4 i) { view = i; }
	void set_time(timing* i) { Time = i; }
	void set_shader(Shader* i) { cube_shader = i; }

private:

	void  updateBuffer_ter() {
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, cube_amount * sizeof(glm::mat4), &cube_matrices[0], GL_STATIC_DRAW);
	}

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

	unsigned int cube_amount;//the amount of cubes that there are to display
	unsigned int buffer;
	Model* cube;
	glm::mat4* cube_matrices;//contains all the cubes mats
	Shader* cube_shader;

	//path finding data
	map_tile** terrian_map;
	unsigned int x_width = 9;// ROW;//rows
	unsigned int z_width = 10;//COL;//collums

};

