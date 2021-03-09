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
#include <string>

#include "object_manger.h"
#include "audio_manger.h"
#include "time.h"


/*
* This class handles all of the animation and updating of possitions 
* it will update the possition of the sounds with the possition of the moving models
*/

enum routine_designation {
	DEFF_ERROR_ROUTINE = 0, DEFF_WORLD_ROUTINE = 1, CHICKEN_ROUTINE = 2, RAIL_ROUTINE = 3,
	CHICKEN_TRANS1_ROUTINE = 4, CHICKEN_TRANS2_ROUTINE = 5, CANNON_ROUTINE = 6, ZAP_TOWER_ROUTINE = 7, CANNON_PLATFORM_ROUTINE = 8,
	ZAP_SPHERE_ROUTINE = 9
};


//this struct controlls the checkpoints for the cart to follow
struct rail_check_point {
	glm::vec3 loc;
	int rail_type;

};

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

	bool rail_network = false;
	std::vector<rail_check_point* > rails;
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

	float move_speed = 30;

	bool being_held = false;
	bool holding_somethig = false;
	actor* held_actor = NULL;
	actor* being_held_by_actor = NULL;

	int animation_section =0;

	float cooldown = 0;
	float cooldown_max = 10;

	bool need_to_turn = false;
	float turn_speed = 1.0f;
	float turn_to = 0.0f;

	//for rail cart
	bool at_start = true;
};

class animation_manager {
public:
	animation_manager();
	~animation_manager();

	void update();
	void update_from_server();
	int turn_object_into_actor(item_info* obje, routine_designation route = DEFF_ERROR_ROUTINE, sound* soun = NULL );

	void define_routine(routine_designation route, int x_min, int z_min, int x_max, int z_max);
	void define_routine(routine_designation route, std::vector< rail_check_point*> points);
	void define_routine(routine_designation route, std::vector<glm::vec3> points);
	void define_routine(routine_designation route, glm::vec3 point);

	void init();

	void print_routines();

	//setters
	void set_cam(Camera* c) { cam = c; }
	void set_time(timing* i) { Time = i; }

	void set_sound_engine(audio_manger* i) { sound_system = i; }
	void set_object_manger(object_manger*i) { OBJM = i; }

	//getters
	std::vector<actor*> *get_actor_list() { return actors; }
	std::vector<routine*>* get_routines_list() { return routines; }

	std::string routine_designation_tostring(routine_designation i);

	void update_actor_id(int id,glm::vec3& loc, glm::vec3& rot,float angle);

private:

	int get_routine_index(routine_designation i);

	void create_nav_points(actor* act, bool wipe_old_points = false);

	int create_chicken_to_fire(bool cursed);

	void create_chicken();

	timing* Time;
	float* deltatime;
	Camera* cam;

	audio_manger *sound_system;
	object_manger* OBJM;

	bool server;

	int id_highest;
	std::vector<int> openIDs;

	std::vector<actor*> *actors;
	std::vector<routine*> *routines;

	int routine_total_predefined;
	actor* cart_waiting_loading_station;
	bool revived_victum;


	bool sending_chicken_to_platform;
	actor* platform;//if there is something on the platform
	actor* flying_chicken;
	//platform
	bool ready_to_lower;
	bool first_plat_cord;
	bool lowering;
	bool raising;

	bool flush;

	bool ready_for_next;

	//zap_o_tron
	bool lower_zap;
	bool ready_to_zap;

	bool zapping;

	float animation_time;
	float animation_time_max;

	bool create_angle_to_fire;
	bool play_sound;
	glm::vec3 cannon_og;
	float x1 = 1;
	float y1 = 0;
	float going_to_x;
	float going_to_z;
	int current_angle;

	float x_t, y_t, z_t;

	std::queue<bool> chickens_to_make;
	std::queue<int> chickens_to_make_angles;
};