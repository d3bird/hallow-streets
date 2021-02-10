#include "animation_manger.h"

animation_manager::animation_manager() {
	Time = NULL;
	deltatime = NULL;
	cam = NULL;

	id_highest = 0;
	routine_total_predefined = 3;
}

animation_manager::~animation_manager() {

	for (int i = 0; i < actors.size(); i++) {
		delete actors[i];
		actors[i] = NULL;
	}
	actors.clear();

	for (int i = 0; i < routines.size(); i++) {
		delete routines[i];
		routines[i] = NULL;
	}
	routines.clear();
}

void animation_manager::init() {
	std::cout << "creating the animation manager"<< std::endl;

	//create the blank routines
	routine* def_routine;

	for (int i = 0; i < routine_total_predefined; i++) {
		def_routine = new routine;
		routines.push_back(def_routine);
	}

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the animation manager" << std::endl;
	}

	std::cout << "finished creating the animation manager" << std::endl;
}

void animation_manager::update() {

	for (int i = 0; i < actors.size(); i++) {
		if (!actors[i]->empty) {

		}
	}

}

//takes a object and a 
int animation_manager::turn_object_into_actor(item_info* obje, sound* soun, int route) {
	actor* new_act = new actor;

	new_act->object = obje;
	new_act->empty = false;

	if (soun == NULL) {
		new_act->soun = NULL;
		new_act->has_sound = false;
	}
	else {
		new_act->soun = soun;
		new_act->has_sound = true;
	}

	if (openIDs.size() >= 1) {
		new_act->id = openIDs[openIDs.size() - 1];

		if (actors[new_act->id]->empty) {
			delete actors[new_act->id];//remove the old
			actors[new_act->id] = new_act;//in with the new
		}
		else {
			std::cout << "actor was not delete proporly, tried to overwrite actor!" << std::endl;
		}

		openIDs.pop_back();
	}
	else {
		new_act->id = id_highest;
		id_highest++;
		actors.push_back(new_act);
	}

	return new_act->id;
}


void animation_manager::define_routine(routine_designation route, int x_min, int z_min, int x_max, int z_max) {

	unsigned int buffer_loc = 0;

	switch (route)
	{
	case DEFF_ERROR_ROUTINE:
		std::cout << "defining DEFF_ERROR_ROUTINE" << std::endl;
		buffer_loc = 0;
		break;
	case DEFF_WORLD_ROUTINE:
		std::cout << "defining DEFF_WORLD_ROUTINE" << std::endl;
		buffer_loc = 1;
		break;
	case CHICKEN_ROUTINE:
		std::cout << "defining CHICKEN_ROUTINE" << std::endl;
		buffer_loc = 2;
		break;
	default:
		std::cout << "not a recognised routine" << std::endl;
		return;
		break;
	}

	if (buffer_loc >= routines.size()) {
		std::cout << "this routine has not been inited" << std::endl;
		return;
	}

	if (routines[buffer_loc]->defined) {
		std::cout << "this routine has already been defined" << std::endl;
		return;
	}
	else {

		routines[buffer_loc]->defined = true;

		//set the area
		routines[buffer_loc]->x_min = x_min;
		routines[buffer_loc]->x_max = x_max;

		routines[buffer_loc]->z_min = z_min;
		routines[buffer_loc]->z_max = z_max;

		int behavior = 0;
		bool flee_player = false;
		float min_flee_distance = -1;
		bool return_area = false;
		//set the specfics

		switch (route)
		{
		case DEFF_ERROR_ROUTINE:
			behavior = 0;
			flee_player = false;
			min_flee_distance = -1;
			return_area = true;
			break;
		case DEFF_WORLD_ROUTINE:
			behavior = 1;
			flee_player = false;
			min_flee_distance = -1;
			return_area = false;
			break;
		case CHICKEN_ROUTINE:
			behavior = 1;
			flee_player = true;
			min_flee_distance = 8;
			return_area = true;
			break;
		}

		routines[buffer_loc]->designation = route;
		routines[buffer_loc]->behavior = behavior;
		routines[buffer_loc]->flee_player = flee_player;
		routines[buffer_loc]->min_flee_distance = min_flee_distance;
		routines[buffer_loc]->return_area = return_area;

	}
}


void animation_manager::print_routines() {

	for (int i = 0; i < routines.size(); i++) {
		if (routines[i] != NULL) {
			if (routines[i]->defined) {
				
				switch (routines[i]->designation)
				{
				case DEFF_ERROR_ROUTINE:
					std::cout << "DEFF_ERROR_ROUTINE designated at " << i << std::endl;
					break;
				case DEFF_WORLD_ROUTINE:
					std::cout << "DEFF_WORLD_ROUTINE designated at " << i << std::endl;
					break;
				case CHICKEN_ROUTINE:
					std::cout << "CHICKEN_ROUTINE designated at " << i << std::endl;
					break;
				default:
					std::cout << "unknown designation at "<<i << std::endl;
					break;
				}
				std::cout << "min x: " << routines[i]->x_min << " max_x: " << routines[i]->x_max << std::endl;
				std::cout << "min z: " << routines[i]->z_min << " max_z: " << routines[i]->z_max << std::endl;

				std::cout << "behavior: " << routines[i]->z_min << routines[i]->behavior << std::endl;
				std::cout << "flee_player: " << routines[i]->z_min << routines[i]->flee_player << std::endl;
				std::cout << "min_flee_distance: " << routines[i]->z_min << routines[i]->min_flee_distance << std::endl;
				std::cout << "return_area: " << routines[i]->z_min << routines[i]->return_area << std::endl;

			}
			else {
				std::cout << "this routine has not been defined" << std::endl;
			}
		}
		std::cout << std::endl;
	}
}
