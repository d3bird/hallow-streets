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
#include "stealth.h"
#include "audio_manger.h"
#include "time.h"
#include "path_finding.h"

#include <physx/PxPhysicsAPI.h>


/*
* This class handles all of the animation and updating of possitions 
* it will update the possition of the sounds with the possition of the moving models
*/

enum routine_designation {
	DEFF_ERROR_ROUTINE = 0, DEFF_WORLD_ROUTINE = 1, CHICKEN_ROUTINE = 2, RAIL_ROUTINE = 3,
	CHICKEN_TRANS1_ROUTINE = 4, CHICKEN_TRANS2_ROUTINE = 5, CANNON_ROUTINE = 6, ZAP_TOWER_ROUTINE = 7, CANNON_PLATFORM_ROUTINE = 8,
	ZAP_SPHERE_ROUTINE = 9, PHYSICS_ROUTINE = 10, LOADING_DOOR_ROUTINE = 11, ROBOT_PATRAL =12
};


//this struct controlls the checkpoints for the cart to follow
struct rail_check_point {
	glm::vec3 loc;
	int rail_type;

};

struct routine{
	bool defined = false;

	routine_designation designation;

	int num_of_actors_using_this = 0;

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

struct door_actor {
	bool activated =true;
	int id;

	routine_designation designation= LOADING_DOOR_ROUTINE;
	item_info* obj = NULL;
	sound* soun = NULL;

	float speed = 5;

	float x_start;
	float y_start;
	float z_start;

	float x_end;
	float y_end;
	float z_end;

	bool toend = true;
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

	//physic info
	physx::PxActor* physics_ret = NULL;
};

struct map_pair {
	int x;
	int z;
};

struct robot_route {
	std::string* name = NULL;
	int id;
	std::vector< map_pair> nav_points;
	bool stationary = false;
	bool look_around = true;
};

struct actor_robot {
	item_info* body;
	item_info* head;

	sound* soun;
	bool has_sound = false;

	std::vector<glm::vec3> nav_points;
	int map_x = 0;
	int map_z = 0;

	int dest_x = 0;
	int dest_z = 0;
	int index =0;//where in the patural it is at
	int id;
	robot_route* routine = NULL;
	bool in_designated_area = true;

	float move_speed = 30;

	int animation_section = 0;

	float cooldown = 0;
	float cooldown_max = 10;

	bool alerted = true;
	float head_angle =0;
	float turn_speed = 6;
	float turn_speed_fast = 18;

	bool need_to_turn = true;
	bool chaseing_player = false;
	bool made_to_start = false;
};

struct distraction {
	bool pick_up_return;//for the chickens
	int x;
	int z;
	float radius = 20;
	float max_time;
	float time_left;
};

struct interactibles {
	item_info* part;

	door_actor* link1;
	door_actor* link2;

	bool activatible;
	bool computer;
	bool pickup;
	float x;
	float z;
};

class animation_manager {
public:
	animation_manager();
	~animation_manager();

	void update();
	void update_from_server();
	int turn_object_into_actor(item_info* obje, routine_designation route = DEFF_ERROR_ROUTINE, bool physics = false, sound* soun = NULL);
	door_actor* turn_object_into_door(item_info* obje, routine_designation route,int direction, sound* soun = NULL);

	void define_routine(routine_designation route, int x_min, int z_min, int x_max, int z_max);
	void define_routine(routine_designation route, std::vector< rail_check_point*> points);
	void define_routine(routine_designation route, std::vector<glm::vec3> points);
	void define_routine(routine_designation route, glm::vec3 point);

	void init();

	void turn_objects_into_actor(item_info* body, item_info* head, int route_id);

	int create_route(std::vector<int> &x_points, std::vector<int> &z_points, std::string* name, bool stat =false);

	void print_routines();

	void create_interactible(item_info* p, bool activatible, bool comp, door_actor* link1 = NULL, door_actor* link2 = NULL);

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

	int get_routine_index(routine_designation i);

	void set_path_finding(path_finding* p) { Pathing = p; }

	void set_stealth(stealth* S) { Stealth = S; }

	void drop_trap(item_info* trap);

	item_info* activate_item();

	void clear_all_object();

	bool show_comp() { return comp_on; }

	void open_doors();
	void close_doors();

	bool are_doors_open() { return doors_open; }
private:
	bool doors_open;
	void init_physics();

	void create_nav_points(actor* act, bool wipe_old_points = false);

	int create_chicken_to_fire(bool cursed);

	void create_chicken();

	void update_doors(float *time);

	void update_robots(float* time);
	void update_distractions(float* time);

	float distance(float x1, float y1, float x2, float y2);
	distraction* is_robot_in_range_of_distraction(actor_robot* robot );
	bool generate_points_for_robot(actor_robot* new_robot, distraction* dist = NULL);

	timing* Time;
	float* deltatime;
	Camera* cam;

	audio_manger *sound_system;
	object_manger* OBJM;

	bool server;

	int id_highest;
	std::vector<int> openIDs;

	std::vector<actor*> *actors;
	std::vector<door_actor*>* actors_doors;
	int door_id = 0;
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

	std::vector< actor_robot*> robots;
	std::vector< robot_route*> robot_routes;

	std::vector< distraction*>distractions;
	std::vector<interactibles*> activatible_items;
	path_finding* Pathing;
	bool comp_on;
	stealth* Stealth;
	int robot_id;
	//physics
	bool update_physics;
	bool init_physics_;
	physx::PxFoundation* f;
	physx::PxCooking* mCooking;
	physx::PxPhysics* p;
	physx::PxPvd* gPvd;
	physx::PxScene* scene;
	physx::PxMaterial* gMaterial;
};