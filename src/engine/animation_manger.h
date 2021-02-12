#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>

#include "object_manger.h"
#include "audio_manger.h"
#include "time.h"


/*
* This class handles all of the animation and updating of possitions 
* it will update the possition of the sounds with the possition of the moving models
*/

enum routine_designation { DEFF_ERROR_ROUTINE = 0, DEFF_WORLD_ROUTINE = 1, CHICKEN_ROUTINE = 2 };

struct routine{
	bool defined = false;

	routine_designation designation;

	int x_min;
	int z_min;

	int x_max;
	int z_max;

	int behavior =0;//determins what the actor does in that in that envirment
	bool flee_player;
	float min_flee_distance;

	bool return_area;//if they get forced out of the area for any reason, this determins if they return 
};

struct actor{
	item_info* object;
	sound* soun;
	std::vector<glm::vec3> nav_points;
	bool has_sound = false;
	int id;
	bool empty = true;
	routine_designation routine;
	bool in_designated_area =true;
};

class animation_manager {
public:
	animation_manager();
	~animation_manager();

	void update();

	int turn_object_into_actor(item_info* obje, routine_designation route = DEFF_ERROR_ROUTINE, sound* soun = NULL );

	void define_routine(routine_designation route, int x_min, int z_min, int x_max, int z_max);

	void init();

	void print_routines();

	void set_cam(Camera* c) { cam = c; }
	void set_time(timing* i) { Time = i; }

	void set_sound_engine(audio_manger* i) { sound_system = i; }
	void set_object_manger(object_manger*i) { OBJM = i; }

private:

	void create_nav_points(actor* act);

	timing* Time;
	float* deltatime;
	Camera* cam;

	audio_manger *sound_system;

	object_manger* OBJM;

	int id_highest;
	std::vector<int> openIDs;

	std::vector<actor*> actors;
	std::vector<routine*> routines;
	int routine_total_predefined;
};