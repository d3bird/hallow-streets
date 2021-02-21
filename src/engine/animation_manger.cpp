#include "animation_manger.h"

animation_manager::animation_manager() {
	Time = NULL;
	deltatime = NULL;
	cam = NULL;
	cart_waiting_loading_station = NULL;
	OBJM = NULL;
	sound_system = NULL;
	id_highest = 0;
	routine_total_predefined = 20;

	flying_chicken = NULL;
	//misc vars
	ready_for_next = true;
	flush = false;
	sending_chicken_to_platform = false;
	lowering = false;
	raising = true;
	ready_to_lower = false;
	platform = NULL;

	first_plat_cord = true;

	//zap vars
	lower_zap = false;
	ready_to_zap = false;
	zapping = false;
	animation_time = 0;
	animation_time_max = 2;

	fire_cannon = false;
	cannon_og = glm::vec3(-1, -1, -1);
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
	case CHICKEN_TRANS1_ROUTINE://chicken to platform
		output = 4;
		break;
	case CHICKEN_TRANS2_ROUTINE://chicken flying through the air
		output = 5;
		break;
	case CANNON_ROUTINE://rotating the cannon
		output = 6;
		break;
	case ZAP_TOWER_ROUTINE://moving down the zap tower and charging the chicken
		output = 7;
		break;
	case CANNON_PLATFORM_ROUTINE://loading the cannon
		output = 8;
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

			if (actors[i]->routine == CANNON_ROUTINE) {
				if (actors[i]->object->angle == actors[i]->turn_to) {
					x1 = going_to_x;
					y1 = going_to_z;
					create_chicken();
					create_nav_points(actors[i]);
				}
				else {
					/*cannon->x_rot = rails[i]->rot.x;
					cannon->y_rot = rails[i]->rot.y;
					cannon->z_rot = rails[i]->rot.z;
					cannon->angle = 180.0f;
					cannon->x = temp_loc.x;
					cannon->y = temp_loc.y;
					cannon->z = temp_loc.z;
					trans = glm::translate(trans, rails[i]->loc);
					trans = glm::rotate(trans, glm::radians(rails[i]->angle), rails[i]->rot);*/
					glm::vec3 current_loc = glm::vec3(actors[i]->object->x, actors[i]->object->y, actors[i]->object->z);
					glm::vec3 current_rot_axis = glm::vec3(actors[i]->object->x_rot, actors[i]->object->y_rot, actors[i]->object->z_rot);

					glm::mat4 trans = glm::mat4(1.0f);
					trans = glm::translate(trans, current_loc);

					bool positive = determin_direction(actors[i]->object->angle, actors[i]->turn_to);
					
					if (positive) {
						actors[i]->object->angle += actors[i]->turn_speed;
						
					}
					else {
						actors[i]->object->angle -= actors[i]->turn_speed;
						
					}

					if (actors[i]->object->angle > 360) {
						actors[i]->object->angle = 0;
					}
					else if (actors[i]->object->angle < 0) {
						actors[i]->object->angle = 360;
					}

					std::cout << "current angle " << actors[i]->object->angle << " turning to " << actors[i]->turn_to << std::endl;
					trans = glm::rotate(trans, glm::radians(actors[i]->object->angle), current_rot_axis);
					update_pak update_pac;

					update_pac.x = current_loc.x;
					update_pac.y = current_loc.y;
					update_pac.z = current_loc.z;

					update_pac.x_scale = 1;
					update_pac.y_scale = 1;
					update_pac.z_scale = 1;

					update_pac.buffer_loc = actors[i]->object->buffer_loc;
					update_pac.item_id = actors[i]->object->item_id;

					OBJM->update_item_matrix(&update_pac, trans);

				}

			}else
			if (actors[i]->being_held) {
				actor* holding = actors[i]->being_held_by_actor;
				glm::vec3 current_loc = glm::vec3(holding->object->x, holding->object->y, holding->object->z);

				current_loc.y += 3;

				
				if (holding->routine == CANNON_PLATFORM_ROUTINE) {//special case for the moving platforms to load the cannon
					current_loc.x += 15;
					current_loc.y += 0.8;
				}

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

				//check if it needs to play a sound if the 
				int route_index = get_routine_index(actors[i]->routine);
				if (routines[route_index]->behavior == 1) {
					glm::vec3 cam_loc = cam->get_pos();
					if ((diff_btwn_pnt(current_loc.x, cam_loc.x) >= 0 && diff_btwn_pnt(current_loc.x, cam_loc.x) <= 10)
						&& (diff_btwn_pnt(current_loc.z, cam_loc.z) >= 0 && diff_btwn_pnt(current_loc.z, cam_loc.z) <= 10)) {
						if (actors[i]->cooldown <= 0) {
							//std::cout << "playing sound" << std::endl;
							sound_system->play_3D_sound(chicken_alarm_call, current_loc);
							actors[i]->cooldown = actors[i]->cooldown_max;
						}
					}
				}

			}else
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
				if(routines[route_index]->behavior ==1){
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
				}


				glm::vec3 nav_point = actors[i]->nav_points[0];
				bool reached_x = false;
				bool reached_y = false;
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

				//move y
				temp = diff_btwn_pnt(current_loc.y, nav_point.y);

				if (speed <= temp) {
					// std::cout << current_loc.x << " " << current_loc.y << " " << current_loc.z << " || " << speed << " <> " << temp << " && " << nav_point.x << std::endl;
					if (determin_direction(current_loc.y, nav_point.y)) {
						current_loc.y += speed;
					}
					else {
						current_loc.y -= speed;
					}
				}
				else {
					//std::cout << "can not move anymore on x axis" << std::endl;
					current_loc.y = nav_point.y;
					reached_y = true;
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

				if (reached_z && reached_x && reached_y) {
					//std::cout << "reached the distination" << i << std::endl;

					if (actors[i]->routine == RAIL_ROUTINE) {

						if (actors[i]->at_start) {
							cart_waiting_loading_station = actors[i];
						}else
						if (!actors[i]->at_start && actors[i]->holding_somethig) {
							if (!sending_chicken_to_platform) {//if there is no chicken on the platform
								std::cout << "updating the cart to unload" << std::endl;
								actors[i]->held_actor->routine = CHICKEN_TRANS1_ROUTINE;
								actors[i]->held_actor->being_held = false;
								actors[i]->held_actor->being_held_by_actor = NULL;

								create_nav_points(actors[i]->held_actor, true);
								actors[i]->holding_somethig = false;
								actors[i]->held_actor = NULL;


								cart_waiting_loading_station = NULL;
								sending_chicken_to_platform = true;
							}
						}
					}
					else if (actors[i]->routine == CHICKEN_TRANS1_ROUTINE) {
						//std::cout << "there is a chicken on the platform" << std::endl;
						platform = actors[i];
					}
					else if (actors[i]->routine == CANNON_PLATFORM_ROUTINE) {
						
						if (ready_for_next && !actors[i]->holding_somethig && platform != NULL) {
							std::cout << "there is a chicken on the platform" << std::endl;
							actors[i]->holding_somethig = true;
							actors[i]->held_actor = platform;

							platform->being_held = true;
							platform->being_held_by_actor = actors[i];
							lower_zap = true;
							ready_for_next = false;
						}

						if (ready_to_lower && actors[i]->holding_somethig) {
							if (lowering) {
								std::cout << "finished lowering down" << std::endl;
								lowering = false;
								raising = true;					
								flush = true;
							}
							else {
								std::cout << "finished rasing up" << std::endl;
								lowering = true;
								raising = false;

								//ready_for_next = true;
							}
						}

						if (platform == NULL) {
							sending_chicken_to_platform = false;
						}

					}
					else if (actors[i]->routine == ZAP_TOWER_ROUTINE ) {
						if (lower_zap) {
							//std::cout << "finished lowering down" << std::endl;
							lower_zap = false;
							ready_to_zap = true;
							zapping = true;
							std::cout << "start of zappng animation" << std::endl;

						}


						if (zapping && ready_to_zap) {
							animation_time += (*deltatime);
							if (animation_time >= animation_time_max) {
								animation_time = 0;
								zapping = false;
								ready_to_zap = false;
								//lower the platform
								ready_to_lower = true;
								lowering = true;

								std::cout << "end of zappng animation" << std::endl;
							}
						}
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

//takes a object and add an animation pattern
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
		move_speed = 5;
		break;
	case RAIL_ROUTINE:
		std::cout << "RAIL_ROUTINE" << std::endl;
		move_speed = 15;
		break;
	case CHICKEN_TRANS1_ROUTINE:
		std::cout << "CHICKEN_TRANS1_ROUTINE" << std::endl;
		break;
	case CHICKEN_TRANS2_ROUTINE:
		std::cout << "CHICKEN_TRANS2_ROUTINE" << std::endl;
		break;
	case CANNON_ROUTINE:
		std::cout << "CANNON_ROUTINE" << std::endl;
		cannon_og = glm::vec3(obje->x, obje->y, obje->z);
		break;
	case ZAP_SPHERE_ROUTINE:
	case ZAP_TOWER_ROUTINE:
		std::cout << "ZAP_TOWER_ROUTINE" << std::endl;
		move_speed = 5;

		break;
	case CANNON_PLATFORM_ROUTINE:
		std::cout << "CANNON_PLATFORM_ROUTINE" << std::endl;
		move_speed = 5;
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
	//std::cout << "defining a route from rail points" << std::endl;

	unsigned int buffer_loc = get_routine_index(route);

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
		break;
	case CHICKEN_TRANS1_ROUTINE:
		std::cout << "defining CHICKEN_TRANS1_ROUTINE" << std::endl;
		break;
	case CANNON_PLATFORM_ROUTINE:
		std::cout << "defining CANNON_PLATFORM_ROUTINE" << std::endl;
		break;
	case ZAP_TOWER_ROUTINE:
		std::cout << "defining ZAP_TOWER_ROUTINE" << std::endl;
		break;
	case ZAP_SPHERE_ROUTINE:
		std::cout << "defining ZAP_SPHERE_ROUTINE" << std::endl;
		break;
	default:
		std::cout << "not a recognised routine" << std::endl;
		return;
		break;
	}

	std::cout << "checkout at buffer_loc: "<<buffer_loc << std::endl;

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
		case CHICKEN_TRANS1_ROUTINE:
			behavior = 4;
			flee_player = false;
			min_flee_distance = -1;
			return_area = true;
			break;
		case CANNON_PLATFORM_ROUTINE:
			behavior = 5;
			flee_player = false;
			min_flee_distance = -1;
			return_area = true;
			break;
		case ZAP_SPHERE_ROUTINE:
		case ZAP_TOWER_ROUTINE:
			behavior = 6;
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
			if (points[i]->rail_type != -1 &&(i == 0 || i == points.size() - 1)) {
				points[i]->rail_type = 3;
			}
			routines[buffer_loc]->rails.push_back(points[i]);
		}
	}

}

void animation_manager::define_routine(routine_designation route, std::vector<glm::vec3> points) {
	//std::cout << "defining a route from many point" << std::endl;
	std::vector<rail_check_point*> output;
	rail_check_point* temp;
	for (int i = 0; i < points.size(); i++) {
		temp = new rail_check_point;
		temp->loc = points[i];
		temp->rail_type = -1;
		output.push_back(temp);
	}

	define_routine(route, output);
}

void animation_manager::define_routine(routine_designation route, glm::vec3 point) {
	//std::cout << "defining a route from a point" << std::endl;
	std::vector<glm::vec3> output;
	output.push_back(point);
	define_routine(route, output);
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

void animation_manager::create_nav_points(actor* act, bool wipe_old_points) {

	if (wipe_old_points) {
		act->nav_points.clear();
	}

	if (act != NULL && act->nav_points.empty()) {

		float dest_x = -1;
		float dest_z = -1;
		float dest_y = 4;
		int index = 0;

		bool multi_points = false;

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
		case CHICKEN_TRANS1_ROUTINE:
			index = 4;
			break;
		case CHICKEN_TRANS2_ROUTINE:
			index = 5;
			break;
		case CANNON_PLATFORM_ROUTINE:
			index = get_routine_index(CANNON_PLATFORM_ROUTINE);
			break;
		case CANNON_ROUTINE:
			index = get_routine_index(CANNON_ROUTINE);
			break;
		case ZAP_SPHERE_ROUTINE:
		case ZAP_TOWER_ROUTINE:
			index = get_routine_index(ZAP_TOWER_ROUTINE);
			//std::cout << "ZAP_TOWER_ROUTINE index: " << index << std::endl;
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


			if (cart_waiting_loading_station != NULL && !cart_waiting_loading_station->holding_somethig) {
				cart_waiting_loading_station->holding_somethig = true;
				cart_waiting_loading_station->held_actor = act;
				act->being_held = true;
				act->being_held_by_actor = cart_waiting_loading_station;
			}

		}
		else if (index == 3) {
			dest_x = routines[index]->rails[0]->loc.x;
			dest_z = routines[index]->rails[0]->loc.z;

			if (cart_waiting_loading_station == NULL && !act->holding_somethig) {
				//cart_waiting_loading_station = act;
				act->at_start = true;
			}

			if (act->holding_somethig /*&& act->object->x == dest_x && act->object->z == dest_z*/) {
				int size = dest_x = routines[index]->rails.size() - 1;
				dest_x = routines[index]->rails[size]->loc.x;
				dest_z = routines[index]->rails[size]->loc.z;
				act->at_start = false;
			}

			//std::cout << dest_x << " " << dest_z << std::endl;

		}
		else if (index == 4) {
			dest_x = routines[index]->rails[0]->loc.x;
			dest_y = routines[index]->rails[0]->loc.y;
			dest_z = routines[index]->rails[0]->loc.z;
		}
		else if (index == 5) {
			//get the end possition 
			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_real_distribution<float> distribution(routines[index]->x_min, routines[index]->x_max);
			dest_x = distribution(mt);
			distribution = std::uniform_real_distribution<float>(routines[index]->z_min, routines[index]->z_max);
			dest_z = distribution(mt);

			multi_points = true;

			//act->object->x = dest_x;
			act->object->y += 10;
			//act->object->z = dest_z;

			//double r = 310.0;
			//int n = 10;
			//for (int i = 0; i < n; i++)
			//{
			//	double x = i * r / n;
			//	double y = sqrt(r * r - x * x);
			//	// both (x,y) and (x,-y) are points on the half-circle
			//}

		}
		else if (index == 6) {
			//rotate the cannon
			multi_points = true;//does not need to move
			std::cout << "updating behavor for the cannon" << std::endl;
			
			if (fire_cannon) {
				act->turn_to = create_chicken_to_fire(true);
			}
			else {

				act->turn_to = 0;

				going_to_z = 0;
				going_to_x = 1;
				std::cout << "creating a new batch of chickens" << std::endl;
				for (int i = 0; i < 5; i++) {
					create_chicken_to_fire(true);
				}

				fire_cannon = true;
			}
		}
		else if (index == 7) {
			if (lower_zap || ready_to_zap) {//lower the platform down
				//std::cout << "setting down" << std::endl;
				dest_x = routines[index]->rails[0]->loc.x;
				dest_y = routines[index]->rails[0]->loc.y - 6;
				dest_z = routines[index]->rails[0]->loc.z;
			}
			else {//raise it back to the def possition
				//std::cout << "setting up" << std::endl;
				dest_x = routines[index]->rails[0]->loc.x;
				dest_y = routines[index]->rails[0]->loc.y;
				dest_z = routines[index]->rails[0]->loc.z;
			}
		}
		else if (index == 8) {
			if (first_plat_cord) {
				dest_x = routines[index]->rails[0]->loc.x;
				dest_y = routines[index]->rails[0]->loc.y;
				dest_z = routines[index]->rails[0]->loc.z;

				act->object->x = dest_x;
				act->object->y = dest_y;
				act->object->z = dest_z;

				first_plat_cord = false;
			}
			else if (ready_to_lower && lowering){//lower the platform down
				dest_x = routines[index]->rails[0]->loc.x;
				dest_y = routines[index]->rails[0]->loc.y - 3;
				dest_z = routines[index]->rails[0]->loc.z;
			}
			else {//raise it back to the def possition
				if (flush) {
					flying_chicken = platform;
					platform->being_held = false;
					platform->being_held_by_actor = NULL;

					act->holding_somethig = false;
					act->held_actor = NULL;

					platform->routine = CHICKEN_ROUTINE;
					create_nav_points(platform, true);
					platform = NULL;


					//ready_for_next = true;
					//flush = false;

					//lowering = false;

					//animation_time = 0;
					//lower_zap = false;
					//zapping = false;
					//ready_to_zap = false;

					ready_for_next = true;
					flush = false;
					sending_chicken_to_platform = false;
					lowering = false;
					raising = true;
					ready_to_lower = false;
					platform = NULL;

					first_plat_cord = true;

					//zap vars
					lower_zap = false;
					ready_to_zap = false;
					zapping = false;
					animation_time = 0;
					animation_time_max = 2;

				}
				dest_x = routines[index]->rails[0]->loc.x;
				dest_y = routines[index]->rails[0]->loc.y;
				dest_z = routines[index]->rails[0]->loc.z;
			}
		}
		if (!multi_points) {
			act->nav_points.push_back(glm::vec3(dest_x, dest_y, dest_z));
		}
	}
}

int animation_manager::create_chicken_to_fire(bool cursed) {
	int index = 2;
	float dest_x = -1;
	float dest_z = -1;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> distribution(routines[index]->x_min, routines[index]->x_max);
	dest_x = distribution(mt);
	distribution = std::uniform_real_distribution<float>(routines[index]->z_min, routines[index]->z_max);
	dest_z = distribution(mt);

	going_to_x = dest_x;
	going_to_z = dest_z;

	float dot = x1 * dest_x + y1 * dest_z;
	float det = x1 * dest_z - y1 * dest_x;
	int angle = atan2(det, dot);

	angle =atan2(dest_z- cannon_og.z, dest_x - cannon_og.x)*(180 / 3.14159265);

	std::cout << "created angle " << angle << std::endl;

	chickens_to_make_angles.push(angle);
	chickens_to_make.push(cursed);

	return angle;
}

void animation_manager::create_chicken() {
	std::cout << "creating a chicken"<<std::endl;

	if (chickens_to_make.empty() || chickens_to_make_angles.empty()) {
		
	}
	else {
		chickens_to_make.pop();
		chickens_to_make_angles.pop();

		if (chickens_to_make.empty()) {
			fire_cannon = false;
		}
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
