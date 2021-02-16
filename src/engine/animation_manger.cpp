#include "animation_manger.h"

animation_manager::animation_manager() {
	Time = NULL;
	deltatime = NULL;
	cam = NULL;

	id_highest = 0;
	routine_total_predefined = 4;
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

int animation_manager::get_routine_index(routine_designation i) {
	int output = 0;

	switch (i)
	{
	case DEFF_ERROR_ROUTINE:
		output = 0;
		break;
	case DEFF_WORLD_ROUTINE:
		output = 1;
		break;
	case CHICKEN_ROUTINE:
		output = 2;
		break;
	case RAIL_ROUTINE:
		output = 3;
		break;
	default:
		break;
	}

	return output;
}



float diff_btwn_pnt(float start, float end) {
	float output = 0;
	if (start < end) {
		output = end - start;
	}
	else {
		output = start - end;
	}
	return output;
}

//determins whther to move the loc in a positive or negative direction
bool determin_direction(float start, float end) {
	if (start < end) {
		return true;//possitive
	}
	return false;//negative
}

void animation_manager::update() {

	float time_mes = (*deltatime);
	float cool = (*deltatime) * 5;
	glm::mat4 trans = glm::mat4(1.0f);

	for (int i = 0; i < actors.size(); i++) {
		if (!actors[i]->empty) {
			//std::cout << "updatting actor "<<i << std::endl;
			float speed = actors[i]->move_speed * time_mes;
			//checkout cooldown

			if (actors[i]->cooldown >= 0) {
				actors[i]->cooldown -= cool;
			}

			if (actors[i]->nav_points.empty()) {//get the next set of nav points
				//std::cout << "out of nav points" << i << std::endl;
				create_nav_points(actors[i]);
			}
			else {//move to the next destination

				glm::vec3 current_loc = glm::vec3(actors[i]->object->x, actors[i]->object->y, actors[i]->object->z);

				int route_index = get_routine_index(actors[i]->routine);

				//std::cout << "updateing " << route_index<<" with behavior "<< routines[route_index]->behavior << std::endl;
				//if (routines[route_index]->behavior == 1) {
					//std::cout << "updateing " << route_index<<" with behavior "<< routines[route_index]->behavior << std::endl;

					//react to the cammera location
					glm::vec3 cam_loc = cam->get_pos();

					if ((diff_btwn_pnt(current_loc.x, cam_loc.x) >= 0 && diff_btwn_pnt(current_loc.x, cam_loc.x) <= 10)
						&& (diff_btwn_pnt(current_loc.z, cam_loc.z) >= 0 && diff_btwn_pnt(current_loc.z, cam_loc.z) <= 10)) {

						if (determin_direction(current_loc.x, cam_loc.x)) {
							actors[i]->nav_points[0].x = current_loc.x - 4.0f;
						}
						else {
							actors[i]->nav_points[0].x = current_loc.x + 4.0f;
						}

						if (determin_direction(current_loc.z, cam_loc.z)) {
							actors[i]->nav_points[0].z = current_loc.z - 4.0f;
						}
						else {
							actors[i]->nav_points[0].z = current_loc.z + 4.0f;
						}

						if (actors[i]->cooldown <= 0) {
							//std::cout << "playing sound" << std::endl;
							sound_system->play_3D_sound(chicken_alarm_call, current_loc);
							actors[i]->cooldown = actors[i]->cooldown_max;
						}
					}
				//}


				glm::vec3 nav_point = actors[i]->nav_points[0];
				bool reached_x = false;
				bool reached_z = false;
				float temp;//remaining distance
				//move x
				temp = diff_btwn_pnt(current_loc.x, nav_point.x);

				if (speed <= temp) {
					// std::cout << current_loc.x << " " << current_loc.y << " " << current_loc.z << " || " << speed << " <> " << temp << " && " << nav_point.x << std::endl;
					if (determin_direction(current_loc.x, nav_point.x)) {
						current_loc.x += speed;
					}
					else {
						current_loc.x -= speed;
					}
				}
				else {
					//std::cout << "can not move anymore on x axis" << std::endl;
					current_loc.x = nav_point.x;
					reached_x = true;
				}

				//move z
				temp = diff_btwn_pnt(current_loc.z, nav_point.z);

				if (speed <= temp) {
					// std::cout << current_loc.x << " " << current_loc.y << " " << current_loc.z << " || " << speed << " <> " << temp << " && " << nav_point.x << std::endl;
					if (determin_direction(current_loc.z, nav_point.z)) {
						current_loc.z += speed;
					}
					else {
						current_loc.z -= speed;
					}
				}
				else {
					//std::cout << "can not move anymore on x axis" << std::endl;
					current_loc.z = nav_point.z;
					reached_z = true;
				}

				//trans = glm::translate(trans, glm::vec3(current_loc.x, current_loc.y, current_loc.z));
				//beast_matrices[all_creatures[i]->get_buffer_loc()] = trans;
				

				//std::cout << "c_loc: " << current_loc.x << " " << current_loc.y << " " << current_loc.z <<" || nav " << nav_point.x << " " << nav_point.y << " " << nav_point.z << std::endl;


				actors[i]->object->x = current_loc.x;
				actors[i]->object->y = current_loc.y;
				actors[i]->object->z = current_loc.z;

				update_pak update_pac;

				update_pac.x = current_loc.x;
				update_pac.y = current_loc.y;
				update_pac.z = current_loc.z;

				update_pac.x_scale = 1;
				update_pac.y_scale = 1;
				update_pac.z_scale = 1;

				update_pac.buffer_loc = actors[i]->object->buffer_loc;
				update_pac.item_id = actors[i]->object->item_id;

				OBJM->update_item_matrix(&update_pac);

				if (reached_z && reached_x) {
					//std::cout << "reached the distination" << i << std::endl;

					switch (actors[i]->routine == RAIL_ROUTINE) {
						actors[i]->at_start = !actors[i]->at_start;
					}

					if(actors[i]->nav_points.size() == 1){
						actors[i]->nav_points.pop_back();
					}
					else {
						actors[i]->nav_points.erase(actors[i]->nav_points.begin());
					}
				}

			}

		}
	}

}

//takes a object and a 
int animation_manager::turn_object_into_actor(item_info* obje, routine_designation route , sound* soun) {
	std::cout << "turning object into an actor following ";

	float move_speed =-1;
	switch (route)
	{
	case DEFF_ERROR_ROUTINE:
		std::cout << "DEFF_ERROR_ROUTINE" << std::endl;
		break;
	case DEFF_WORLD_ROUTINE:
		std::cout << "DEFF_WORLD_ROUTINE" << std::endl;
		break;
	case CHICKEN_ROUTINE:
		std::cout << "CHICKEN_ROUTINE" << std::endl;
		break;
	case RAIL_ROUTINE:
		std::cout << "RAIL_ROUTINE" << std::endl;
		move_speed = 15;
		break;
	default:
		break;
	}

	actor* new_act = new actor;

	if (move_speed != -1) {
		new_act->move_speed = move_speed;
	}

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

	new_act->routine = route;

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


void animation_manager::define_routine(routine_designation route, std::vector< rail_check_point*> points) {
	
	unsigned int buffer_loc = 0;

	switch (route)
	{
	case DEFF_ERROR_ROUTINE:
		std::cout << "cannot define DEFF_ERROR_ROUTINE to a bunch of rail" << std::endl;
		return;
		break;
	case DEFF_WORLD_ROUTINE:
		std::cout << "cannot define DEFF_ERROR_ROUTINE to a bunch of rail" << std::endl;
		return;
		break;
	case CHICKEN_ROUTINE:
		std::cout << "cannot define DEFF_ERROR_ROUTINE to a bunch of rail" << std::endl;
		return;
		break;
	case RAIL_ROUTINE:
		std::cout << "defining RAIL_ROUTINE" << std::endl;
		buffer_loc = 3;
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

		int behavior = 0;
		bool flee_player = false;
		float min_flee_distance = -1;
		bool return_area = false;
		//set the specfics

		switch (route)
		{
		case RAIL_ROUTINE:
			behavior = 3;
			flee_player = false;
			min_flee_distance = -1;
			return_area = true;
			break;
		}

		routines[buffer_loc]->designation = route;
		routines[buffer_loc]->behavior = behavior;
		routines[buffer_loc]->flee_player = flee_player;
		routines[buffer_loc]->min_flee_distance = min_flee_distance;
		routines[buffer_loc]->return_area = return_area;
		routines[buffer_loc]->rail_network = true;


		for (int i = 0; i < points.size(); i++) {
			if (i == 0 || i == points.size() - 1) {
				points[i]->rail_type = 3;
			}
			routines[buffer_loc]->rails.push_back(points[i]);
		}
	}

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
	case RAIL_ROUTINE:
std::cout << "can not define a RAIL_ROUTINE because needs points of rails" << std::endl;
return;
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

void animation_manager::create_nav_points(actor* act) {

	if (act != NULL && act->nav_points.empty()) {

		float dest_x = -1;
		float dest_z = -1;

		int index = 0;

		switch (act->routine)
		{
		case CHICKEN_ROUTINE:
			index = 2;
			break;
		case RAIL_ROUTINE:
			index = 3;
			//std::cout << "updating cart" << std::endl;

			//std::cout << act->object->x <<" "<< act->object->z<< std::endl;
			break;
		case DEFF_ERROR_ROUTINE:
		case DEFF_WORLD_ROUTINE:
		default:
			index = 0;
			break;
		}

		if (index == 2) {
			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_real_distribution<float> distribution(routines[index]->x_min, routines[index]->x_max);
			dest_x = distribution(mt);
			distribution = std::uniform_real_distribution<float>(routines[index]->z_min, routines[index]->z_max);
			dest_z = distribution(mt);

		}
		else if (index == 3) {
			dest_x = routines[index]->rails[0]->loc.x;
			dest_z = routines[index]->rails[0]->loc.z;
			if (act->object->x == dest_x && act->object->z == dest_z) {
				int size = dest_x = routines[index]->rails.size() - 1;
				dest_x = routines[index]->rails[size]->loc.x;
				dest_z = routines[index]->rails[size]->loc.z;
			}

			//std::cout << dest_x << " " << dest_z << std::endl;

		}

		act->nav_points.push_back(glm::vec3(dest_x, 4, dest_z));

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
