#include "animation_manger.h"

using namespace physx;

animation_manager::animation_manager() {
	Time = NULL;
	deltatime = NULL;
	cam = NULL;
	cart_waiting_loading_station = NULL;
	OBJM = NULL;
	sound_system = NULL;
	id_highest = 0;
	routine_total_predefined = 20;

	actors = new std::vector<actor*>();
	actors_doors = new std::vector<door_actor*>();
	routines = new std::vector<routine*>();

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

	play_sound = false;
	create_angle_to_fire = false;
	cannon_og = glm::vec3(-1, -1, -1);
	server = true;

	//physics setters
	scene = NULL;
	p = NULL;
	f = NULL;

	update_physics = false;
	init_physics_ = false;

	//robot setters
	Pathing = NULL;
	Stealth = NULL;
	robot_id = 0;
	comp_on = false;
	doors_open = false;
}

animation_manager::~animation_manager() {

	//for (int i = 0; i < actors[0]->size(); i++) {
	//	delete actors[0][i];
	//	actors[0][i] = NULL;
	//}
	//actors[0].clear();

	//for (int i = 0; i < routines[0].size(); i++) {
	//	delete routines[0][i];
	//	routines[0][i] = NULL;
	//}
	//routines[0].clear();

	//physics
	f->release();
	p->release();
	scene->release();
}

void animation_manager::init() {
	std::cout << "creating the animation manager"<< std::endl;


	//create the blank routines[0]
	routine* def_routine;

	for (int i = 0; i < routine_total_predefined; i++) {
		def_routine = new routine;
		routines[0].push_back(def_routine);
	}
	//Severity	Code	Description	Project	File	Line	Suppression State
//	Error	LNK2019	unresolved external symbol "public: __cdecl physx::PxDefaultErrorCallback::PxDefaultErrorCallback(void)" (? ? 0PxDefaultErrorCallback@physx@@QEAA@XZ) referenced in function "private: void __cdecl animation_manager::init_physics(void)" (? init_physics@animation_manager@@AEAAXXZ)	engine	C : \Users\dogbi\OneDrive\Desktop\hallow_streets\build\animation_manger.obj	1

	/*if (OBJM == NULL) {
		Stealth = new stealth();
		Stealth->init(OBJM);
	}
	else {
		Stealth = NULL;
	}*/

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
	std::cout << "there was a problem getting time in the animation manager" << std::endl;
	}
	if (init_physics_) {
		init_physics();
	}
	std::cout << "finished creating the animation manager" << std::endl;
}

void animation_manager::init_physics() {
	std::cout << "creating the physics engine" << std::endl;
	//physx::
	physx::PxDefaultAllocator a;
	physx::PxDefaultErrorCallback e;

	//f = PxCreateFoundation(PX_PHYSICS_VERSION, a, e);
	f = PxCreateFoundation(PX_PHYSICS_VERSION,a,e);
	/*mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *f, PxCookingParams(scale));
	if (!mCooking) {
		std::cout << ("PxCreateCooking failed!") << std::endl;
	}*/
	
	//gPvd = PxCreatePvd(*f);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	p = PxCreatePhysics(PX_PHYSICS_VERSION, *f, physx::PxTolerancesScale());

	/*if (!PxInitExtensions(*p, mPvd)) {
		std::cout << ("PxInitExtensions failed!") << std::endl;
	}*/
	physx::PxSceneDesc sc(p->getTolerancesScale());

	sc.filterShader = physx::PxDefaultSimulationFilterShader;
	sc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

	scene = p->createScene(sc);

	gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*p, physx::PxPlane(0, 0, 1, 0), *gMaterial);//

	//scene->addActor(*groundPlane);

	/*
	creating the physics engine
C:\Users\dogbi\OneDrive\Desktop\imgui\PhysX\physx\source\physx\src\NpPhysics.cpp (259) : invalid parameter : Physics::createScene: desc.isValid() is false!

	*/

	physx::PxShape* shape = p->createShape(physx::PxBoxGeometry(1, 1, 1), *gMaterial, true);
	//could use the axis aligned bounding box dimensions for a box geometry?
	physx::PxTransform t({ 0,0,0 });
	physx::PxActor* a_new;
	a_new = p->createRigidDynamic(t);

	std::cout << "finished creating the physics engine" << std::endl;

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

std::string animation_manager::routine_designation_tostring(routine_designation i) {
	std::string output = "unkown";
	switch (i)
	{
	case DEFF_ERROR_ROUTINE:
		output = "DEFF_ERROR_ROUTINE";
		break;
	case DEFF_WORLD_ROUTINE:
		output = "DEFF_WORLD_ROUTINE";
		break;
	case CHICKEN_ROUTINE:
		output = "CHICKEN_ROUTINE";
		break;
	case RAIL_ROUTINE:
		output = "RAIL_ROUTINE";
		break;
	case CHICKEN_TRANS1_ROUTINE://chicken to platform
		output = "CHICKEN_TRANS1_ROUTINE";
		break;
	case CHICKEN_TRANS2_ROUTINE://chicken flying through the air
		output = "CHICKEN_TRANS2_ROUTINE";
		break;
	case CANNON_ROUTINE://rotating the cannon
		output = "CANNON_ROUTINE";
		break;
	case ZAP_TOWER_ROUTINE://moving down the zap tower and charging the chicken
		output = "ZAP_TOWER_ROUTINE";
		break;
	case CANNON_PLATFORM_ROUTINE://loading the cannon
		output = "CANNON_PLATFORM_ROUTINE";
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


void animation_manager::update_from_server() {
	//std::cout << "updating from server" << std::endl;
}

void animation_manager::update() {


	if (!server) {
		update_from_server();
		return;
	}



	float time_mes = (*deltatime);
	float cool = (*deltatime) * 5;

	update_doors(&time_mes);
	update_robots(&time_mes);

	glm::mat4 trans = glm::mat4(1.0f);

	if (init_physics_ &&update_physics) {
		if (scene != NULL) {
			scene->simulate(1.0f / 60.0f);
			scene->fetchResults(true);
		}
		else {
			std::cout << "scene was null" << std::endl;
		}
	}

	for (int i = 0; i < actors[0].size(); i++) {
		if (!actors[0][i]->empty) {
			//std::cout << "updatting actor "<<i << std::endl;
			float speed = actors[0][i]->move_speed * time_mes;
			//checkout cooldown

			if (actors[0][i]->cooldown >= 0) {
				actors[0][i]->cooldown -= cool;
			}

			if (actors[0][i]->routine == CANNON_ROUTINE) {
				if (actors[0][i]->object->angle == actors[0][i]->turn_to) {
					x1 = going_to_x;
					y1 = going_to_z;
					if (play_sound) {
						create_chicken();//fire the chicken
						//std::cout << "playing the sound of the cannon" << std::endl;
						sound_system->play_sound_effect(Explosion_Large_Blast_1);
						play_sound = false;
					}
					
					create_nav_points(actors[0][i]);
				}
				else {

					glm::vec3 current_loc = glm::vec3(actors[0][i]->object->x, actors[0][i]->object->y, actors[0][i]->object->z);
					glm::vec3 current_rot_axis = glm::vec3(actors[0][i]->object->x_rot, actors[0][i]->object->y_rot, actors[0][i]->object->z_rot);

					glm::mat4 trans = glm::mat4(1.0f);
					trans = glm::translate(trans, current_loc);

					bool positive = determin_direction(actors[0][i]->object->angle, actors[0][i]->turn_to);
					
					if (positive) {
						actors[0][i]->object->angle += actors[0][i]->turn_speed;
						
					}
					else {
						actors[0][i]->object->angle -= actors[0][i]->turn_speed;
						
					}

					if (actors[0][i]->object->angle > 360) {
						actors[0][i]->object->angle = 0;
					}
					else if (actors[0][i]->object->angle < 0) {
						actors[0][i]->object->angle = 360;
					}

				//	std::cout << "current angle " << actors[0][i]->object->angle << " turning to " << actors[0][i]->turn_to << std::endl;
					trans = glm::rotate(trans, glm::radians(actors[0][i]->object->angle), current_rot_axis);
					update_pak update_pac;

					update_pac.x = current_loc.x;
					update_pac.y = current_loc.y;
					update_pac.z = current_loc.z;

					update_pac.x_scale = 1;
					update_pac.y_scale = 1;
					update_pac.z_scale = 1;

					update_pac.buffer_loc = actors[0][i]->object->buffer_loc;
					update_pac.item_id = actors[0][i]->object->item_id;

					OBJM->update_item_matrix(&update_pac, trans);


				}

			}else
			if (actors[0][i]->being_held) {
				actor* holding = actors[0][i]->being_held_by_actor;
				glm::vec3 current_loc = glm::vec3(holding->object->x, holding->object->y, holding->object->z);

				current_loc.y += 3;

				
				if (holding->routine == CANNON_PLATFORM_ROUTINE) {//special case for the moving platforms to load the cannon
					current_loc.x += 15;
					current_loc.y += 0.8;
				}

				actors[0][i]->object->x = current_loc.x;
				actors[0][i]->object->y = current_loc.y;
				actors[0][i]->object->z = current_loc.z;


				update_pak update_pac;

				update_pac.x = current_loc.x;
				update_pac.y = current_loc.y;
				update_pac.z = current_loc.z;

				update_pac.x_scale = 1;
				update_pac.y_scale = 1;
				update_pac.z_scale = 1;

				update_pac.buffer_loc = actors[0][i]->object->buffer_loc;
				update_pac.item_id = actors[0][i]->object->item_id;

				OBJM->update_item_matrix(&update_pac);

				//check if it needs to play a sound if the 
				int route_index = get_routine_index(actors[0][i]->routine);
				if (routines[0][route_index]->behavior == 1) {
					glm::vec3 cam_loc = cam->get_pos();
					if ((diff_btwn_pnt(current_loc.x, cam_loc.x) >= 0 && diff_btwn_pnt(current_loc.x, cam_loc.x) <= 10)
						&& (diff_btwn_pnt(current_loc.z, cam_loc.z) >= 0 && diff_btwn_pnt(current_loc.z, cam_loc.z) <= 10)) {
						if (actors[0][i]->cooldown <= 0) {
							//std::cout << "playing sound" << std::endl;
							sound_system->play_3D_sound(chicken_alarm_call, current_loc);
							actors[0][i]->cooldown = actors[0][i]->cooldown_max;
						}
					}
				}

			}else
			if (actors[0][i]->nav_points.empty()) {//get the next set of nav points
				//std::cout << "out of nav points" << i << std::endl;
				create_nav_points(actors[0][i]);
			}
			else {//move to the next destination

				glm::vec3 current_loc = glm::vec3(actors[0][i]->object->x, actors[0][i]->object->y, actors[0][i]->object->z);

				int route_index = get_routine_index(actors[0][i]->routine);

				//std::cout << "updateing " << route_index<<" with behavior "<< routines[0][route_index]->behavior << std::endl;
				//if (routines[0][route_index]->behavior == 1) {
					//std::cout << "updateing " << route_index<<" with behavior "<< routines[0][route_index]->behavior << std::endl;

					//react to the cammera location
				if(routines[0][route_index]->behavior ==1){
					glm::vec3 cam_loc = cam->get_pos();

					if ((diff_btwn_pnt(current_loc.x, cam_loc.x) >= 0 && diff_btwn_pnt(current_loc.x, cam_loc.x) <= 10)
						&& (diff_btwn_pnt(current_loc.z, cam_loc.z) >= 0 && diff_btwn_pnt(current_loc.z, cam_loc.z) <= 10)) {

						if (determin_direction(current_loc.x, cam_loc.x)) {
							actors[0][i]->nav_points[0].x = current_loc.x - 4.0f;
						}
						else {
							actors[0][i]->nav_points[0].x = current_loc.x + 4.0f;
						}

						if (determin_direction(current_loc.z, cam_loc.z)) {
							actors[0][i]->nav_points[0].z = current_loc.z - 4.0f;
						}
						else {
							actors[0][i]->nav_points[0].z = current_loc.z + 4.0f;
						}

						if (actors[0][i]->cooldown <= 0) {
							//std::cout << "playing sound" << std::endl;
							sound_system->play_3D_sound(chicken_alarm_call, current_loc);
							actors[0][i]->cooldown = actors[0][i]->cooldown_max;
						}
					}
				}


				glm::vec3 nav_point = actors[0][i]->nav_points[0];
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


				actors[0][i]->object->x = current_loc.x;
				actors[0][i]->object->y = current_loc.y;
				actors[0][i]->object->z = current_loc.z;

				update_pak update_pac;

				update_pac.x = current_loc.x;
				update_pac.y = current_loc.y;
				update_pac.z = current_loc.z;

				update_pac.x_scale = 1;
				update_pac.y_scale = 1;
				update_pac.z_scale = 1;

				update_pac.buffer_loc = actors[0][i]->object->buffer_loc;
				update_pac.item_id = actors[0][i]->object->item_id;

				OBJM->update_item_matrix(&update_pac);

				if (reached_z && reached_x && reached_y) {
					//std::cout << "reached the distination" << i << std::endl;

					if (actors[0][i]->routine == RAIL_ROUTINE) {

						if (actors[0][i]->at_start) {
							cart_waiting_loading_station = actors[0][i];
						}else
						if (!actors[0][i]->at_start && actors[0][i]->holding_somethig) {
							if (!sending_chicken_to_platform) {//if there is no chicken on the platform
								//std::cout << "updating the cart to unload" << std::endl;
								actors[0][i]->held_actor->routine = CHICKEN_TRANS1_ROUTINE;
								actors[0][i]->held_actor->being_held = false;
								actors[0][i]->held_actor->being_held_by_actor = NULL;

								create_nav_points(actors[0][i]->held_actor, true);
								actors[0][i]->holding_somethig = false;
								actors[0][i]->held_actor = NULL;


								cart_waiting_loading_station = NULL;
								sending_chicken_to_platform = true;
							}
						}
					}
					else if (actors[0][i]->routine == CHICKEN_TRANS1_ROUTINE) {
						//std::cout << "there is a chicken on the platform" << std::endl;
						platform = actors[0][i];
					}
					else if (actors[0][i]->routine == CHICKEN_TRANS2_ROUTINE) {
						//std::cout << "there is a chicken on the platform" << std::endl;
						actors[0][i]->routine = CHICKEN_ROUTINE;
					}
					else if (actors[0][i]->routine == CANNON_PLATFORM_ROUTINE) {
						
						if (ready_for_next && !actors[0][i]->holding_somethig && platform != NULL) {
						//	std::cout << "there is a chicken on the platform" << std::endl;
							actors[0][i]->holding_somethig = true;
							actors[0][i]->held_actor = platform;

							platform->being_held = true;
							platform->being_held_by_actor = actors[0][i];
							lower_zap = true;
							ready_for_next = false;
						}

						if (ready_to_lower && actors[0][i]->holding_somethig) {
							if (lowering) {
							//	std::cout << "finished lowering down" << std::endl;
								lowering = false;
								raising = true;					
								flush = true;
							}
							else {
								//std::cout << "finished rasing up" << std::endl;
								lowering = true;
								raising = false;

								//ready_for_next = true;
							}
						}

						if (platform == NULL) {
							sending_chicken_to_platform = false;
						}

					}
					else if (actors[0][i]->routine == ZAP_TOWER_ROUTINE ) {
						if (lower_zap) {
							//std::cout << "finished lowering down" << std::endl;
							lower_zap = false;
							ready_to_zap = true;
							zapping = true;
							//std::cout << "start of zappng animation" << std::endl;

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
								create_angle_to_fire = true;
								//std::cout << "end of zappng animation" << std::endl;
							}
						}
					}


					if(actors[0][i]->nav_points.size() == 1){
						actors[0][i]->nav_points.pop_back();
					}
					else {
						actors[0][i]->nav_points.erase(actors[0][i]->nav_points.begin());
					}
				}

			}

		}
	}

	update_distractions(&time_mes);
}

void animation_manager::update_doors(float* time) {
	for (int i = 0; i < actors_doors[0].size(); i++) {

		if (actors_doors[0][i]->activated) {
			glm::vec3 current_loc = glm::vec3(actors_doors[0][i]->obj->x, actors_doors[0][i]->obj->y, actors_doors[0][i]->obj->z);
			float speed = actors_doors[0][i]->speed * (*time);


			glm::vec3 nav_point;
			if (actors_doors[0][i]->toend) {
				nav_point = glm::vec3(actors_doors[0][i]->x_end, actors_doors[0][i]->y_end, actors_doors[0][i]->z_end);
			}
			else {
				nav_point = glm::vec3(actors_doors[0][i]->x_start, actors_doors[0][i]->y_start, actors_doors[0][i]->z_start);
			}

			/*door_actor* new_act = actors_doors[0][i];
			std::cout << "start loc x: " << new_act->x_start << " y: " << new_act->y_start << " z: " << new_act->z_start << std::endl;
			std::cout << "end loc x: " << new_act->x_end << " y: " << new_act->y_end << " z: " << new_act->z_end << std::endl;

			std::cout << current_loc.x << " to " << nav_point.x << std::endl;
			std::cout << current_loc.z << " to " << nav_point.z << std::endl;*/

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


			actors_doors[0][i]->obj->x = current_loc.x;
			actors_doors[0][i]->obj->y = current_loc.y;
			actors_doors[0][i]->obj->z = current_loc.z;

			update_pak update_pac;

			update_pac.x = current_loc.x;
			update_pac.y = current_loc.y;
			update_pac.z = current_loc.z;

			update_pac.x_scale = 1;
			update_pac.y_scale = 1;
			update_pac.z_scale = 1;

			update_pac.angle = actors_doors[0][i]->obj->angle;

			update_pac.buffer_loc = actors_doors[0][i]->obj->buffer_loc;
			update_pac.item_id = actors_doors[0][i]->obj->item_id;

			OBJM->update_item_matrix(&update_pac);

			if (reached_z && reached_x && reached_y) {
				actors_doors[0][i]->toend = !actors_doors[0][i]->toend;
				actors_doors[0][i]->activated = false;
			}
		}
	}
}

//takes a object and add an animation pattern
int animation_manager::turn_object_into_actor(item_info* obje, routine_designation route , bool physics, sound* soun) {
	//std::cout << "turning object into an actor following ";
	bool creating_physics_object = false;
	float move_speed =-1;
	switch (route)
	{
	case DEFF_ERROR_ROUTINE:
	//	std::cout << "DEFF_ERROR_ROUTINE" << std::endl;
		break;
	case DEFF_WORLD_ROUTINE:
		//std::cout << "DEFF_WORLD_ROUTINE" << std::endl;
		break;
	case CHICKEN_ROUTINE:
		//std::cout << "CHICKEN_ROUTINE" << std::endl;
		move_speed = 5;
		break;
	case RAIL_ROUTINE:
		//std::cout << "RAIL_ROUTINE" << std::endl;
		move_speed = 15;
		break;
	case CHICKEN_TRANS1_ROUTINE:
		//std::cout << "CHICKEN_TRANS1_ROUTINE" << std::endl;
		break;
	case CHICKEN_TRANS2_ROUTINE:
		//std::cout << "CHICKEN_TRANS2_ROUTINE" << std::endl;
		move_speed = 5;
		break;
	case CANNON_ROUTINE:
		//std::cout << "CANNON_ROUTINE" << std::endl;
		cannon_og = glm::vec3(obje->x, obje->y, obje->z);
		break;
	case ZAP_SPHERE_ROUTINE:
	case ZAP_TOWER_ROUTINE:
		//std::cout << "ZAP_TOWER_ROUTINE" << std::endl;
		move_speed = 5;

		break;
	case CANNON_PLATFORM_ROUTINE:
		//std::cout << "CANNON_PLATFORM_ROUTINE" << std::endl;
		move_speed = 5;
		break;
	case PHYSICS_ROUTINE:
		creating_physics_object = true;
		break;
	default:
		break;
	}

	if (creating_physics_object) {

		return -1;
	}
	else {
		int index = get_routine_index(route);
		routines[0][index]->num_of_actors_using_this++;
		routines[0][index]->designation = route;
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

			if (actors[0][new_act->id]->empty) {
				delete actors[0][new_act->id];//remove the old
				actors[0][new_act->id] = new_act;//in with the new
			}
			else {
				std::cout << "actor was not delete proporly, tried to overwrite actor!" << std::endl;
			}

			openIDs.pop_back();
		}
		else {
			new_act->id = id_highest;
			id_highest++;
			actors[0].push_back(new_act);
		}

		if (route == CHICKEN_TRANS2_ROUTINE) {
			create_nav_points(new_act, true);
		}


		if (physics) {
			new_act->physics_ret;

			//physx::PxRigidDynamic* aCapsuleActor = thePhysics->createRigidDynamic(physx::PxTransform(position));
			//physx::PxTransform relativePose(PxQuat(PxHalfPi, PxVec(0, 0, 1)));
			//physx::PxShape* aCapsuleShape = physx::PxRigidActorExt::createExclusiveShape(*aCapsuleActor,
			//	physx::PxCapsuleGeometry(radius, halfHeight), aMaterial);
			//aCapsuleShape->setLocalPose(relativePose);
			//physx::PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, capsuleDensity);
			//scene->addActor(aCapsuleActor);

			//scene->addActor(*(new_act->physics_ret), physx::createRigidDynamic());
		}

		return new_act->id;
	}
}

door_actor* animation_manager::turn_object_into_door(item_info* obje, routine_designation route, int direction, sound* soun) {
	door_actor* new_act = new door_actor;
	new_act->id = door_id;
	door_id++;
	new_act->designation = route;
	new_act->activated = false;
	new_act->obj = obje;
	new_act->soun = soun;

	new_act->x_start = obje->x;
	new_act->y_start = obje->y;
	new_act->z_start = obje->z;

	new_act->x_end = obje->x;
	new_act->y_end = obje->y;
	new_act->z_end = obje->z;

	switch (direction) {
	case 0://east
		new_act->x_end += 4;
		break;
	case 1://west
		new_act->x_end -= 4;
		break;
	case 2://south
		new_act->z_end += 4;
		break;
	case 3://north
		new_act->z_end -= 4;
		break;
	}

	std::cout << "created door" << std::endl;
	std::cout << "start loc x: "<< new_act->x_start<<" y: "<< new_act->y_start<<" z: "<< new_act->z_start << std::endl;
	std::cout << "end loc x: " << new_act->x_end << " y: " << new_act->y_end << " z: " << new_act->z_end << std::endl;
	actors_doors->push_back(new_act);
	return new_act;
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

	if (buffer_loc >= routines[0].size()) {
		std::cout << "this routine has not been inited" << std::endl;
		return;
	}

	if (routines[0][buffer_loc]->defined) {
		std::cout << "this routine has already been defined" << std::endl;
		return;
	}
	else {

		routines[0][buffer_loc]->defined = true;

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

		routines[0][buffer_loc]->designation = route;
		routines[0][buffer_loc]->behavior = behavior;
		routines[0][buffer_loc]->flee_player = flee_player;
		routines[0][buffer_loc]->min_flee_distance = min_flee_distance;
		routines[0][buffer_loc]->return_area = return_area;
		routines[0][buffer_loc]->rail_network = true;


		for (int i = 0; i < points.size(); i++) {
			if (points[i]->rail_type != -1 &&(i == 0 || i == points.size() - 1)) {
				points[i]->rail_type = 3;
			}
			routines[0][buffer_loc]->rails.push_back(points[i]);
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

	if (buffer_loc >= routines[0].size()) {
		std::cout << "this routine has not been inited" << std::endl;
		return;
	}

	if (routines[0][buffer_loc]->defined) {
		std::cout << "this routine has already been defined" << std::endl;
		return;
	}
	else {
		routines[0][buffer_loc]->defined = true;

		//set the area
		routines[0][buffer_loc]->x_min = x_min;
		routines[0][buffer_loc]->x_max = x_max;

		routines[0][buffer_loc]->z_min = z_min;
		routines[0][buffer_loc]->z_max = z_max;

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

		routines[0][buffer_loc]->designation = route;
		routines[0][buffer_loc]->behavior = behavior;
		routines[0][buffer_loc]->flee_player = flee_player;
		routines[0][buffer_loc]->min_flee_distance = min_flee_distance;
		routines[0][buffer_loc]->return_area = return_area;

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
			std::uniform_real_distribution<float> distribution(routines[0][index]->x_min, routines[0][index]->x_max);
			dest_x = distribution(mt);
			distribution = std::uniform_real_distribution<float>(routines[0][index]->z_min, routines[0][index]->z_max);
			dest_z = distribution(mt);


			if (cart_waiting_loading_station != NULL && !cart_waiting_loading_station->holding_somethig) {
				cart_waiting_loading_station->holding_somethig = true;
				cart_waiting_loading_station->held_actor = act;
				act->being_held = true;
				act->being_held_by_actor = cart_waiting_loading_station;
			}

		}
		else if (index == 3) {
			dest_x = routines[0][index]->rails[0]->loc.x;
			dest_z = routines[0][index]->rails[0]->loc.z;

			if (cart_waiting_loading_station == NULL && !act->holding_somethig) {
				//cart_waiting_loading_station = act;
				act->at_start = true;
			}

			if (act->holding_somethig /*&& act->object->x == dest_x && act->object->z == dest_z*/) {
				int size = dest_x = routines[0][index]->rails.size() - 1;
				dest_x = routines[0][index]->rails[size]->loc.x;
				dest_z = routines[0][index]->rails[size]->loc.z;
				act->at_start = false;
			}

			//std::cout << dest_x << " " << dest_z << std::endl;

		}
		else if (index == 4) {
			dest_x = routines[0][index]->rails[0]->loc.x;
			dest_y = routines[0][index]->rails[0]->loc.y;
			dest_z = routines[0][index]->rails[0]->loc.z;
		}
		else if (index == 5) {
			//get the end possition 
			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_real_distribution<float> distribution(routines[0][index]->x_min, routines[0][index]->x_max);
			dest_x = distribution(mt);
			distribution = std::uniform_real_distribution<float>(routines[0][index]->z_min, routines[0][index]->z_max);
			dest_z = distribution(mt);

			dest_x = going_to_x;

			dest_z = going_to_z;
			multi_points = true;

			//act->object->x = dest_x;
			//act->object->y += 10;
			//act->object->z = dest_z;

			x_t = act->object->x;
			y_t = act->object->y;
			z_t = act->object->z;

			float x_distance = diff_btwn_pnt(x_t, dest_x);
			float y_distance = diff_btwn_pnt(y_t, dest_y);
			float z_distance = diff_btwn_pnt(z_t, dest_z);
			float magnitude = sqrt((x_distance * x_distance) + (y_distance * y_distance) + (z_distance * z_distance));

			
			glm::vec3 center = glm::vec3((x_t+ dest_x) / 2, (y_t + dest_y) / 2, (z_t + dest_z) / 2);

			float temp_x = dest_x - x_t;
			float temp_y = dest_y - y_t;
			float temp_z = dest_z - z_t;

			float radius = sqrt((temp_x * temp_x) + (temp_y * temp_y) + (temp_z * temp_z));
			float diamiter = radius * 2;

			/*std::cout << std::endl;
			std::cout << "information to creates the arcs" << std::endl;
			std::cout << "start: x " << x_t << " y " << y_t << " z " << z_t << std::endl;
			std::cout << "center: x " << center.x << " y " << center.y << " z " << center.z << std::endl;
			std::cout << "end: x " << dest_x << " y " << dest_y << " z " << dest_z << std::endl;
			std::cout << "radius " << radius << std::endl;*/

			int points = 10;
			int angle_dec = 180/ points;

			float temp_r = radius /18;

			float y = y_t;
			float x = x_t;
			float z = z_t;

			int mul = 1;

			bool pos_x = determin_direction(x_t, center.x);
			bool pos_z = determin_direction(z_t, center.z);

			

			float temp_r_x = radius *cos(current_angle);
			float temp_r_z = radius *sin(current_angle);

			temp_r_x /= points;
			temp_r_z /= points;

			for (int i = 0; i < points; i++) {
				x += temp_r_x;
				z += temp_r_z;
				/*if (pos_x) {
					x += temp_r_x;
				}
				else {
					x -= temp_r_x;

				}
				if(pos_z){
					z += temp_r_z;
				}
				else {
					z -= temp_r_z;

				}*/
			//	y = temp_r*sin(i);

					/*if (determin_direction(x, dest_x)) {
						x += incrment_distance;
					}
					else {
						x -= incrment_distance;
					}

					if (determin_direction(z, dest_z)) {
						z += incrment_distance;
					}
					else {
						z -= incrment_distance;
					}*/

					//std::cout << "nav_point: x " << x << " y " << y << " z " << z << " (i  " << i << ")" << std::endl;
					
				act->nav_points.push_back(glm::vec3(x, y, z));
				mul++;
			}

			//std::cout <<" points generated "<< act->nav_points.size()<< std::endl;
			//std::cout << std::endl;

			act->nav_points.push_back(glm::vec3(dest_x, dest_y, dest_z));

		}
		else if (index == 6) {
			//rotate the cannon
			multi_points = true;//does not need to move
			//std::cout << "updating behavor for the cannon" << std::endl;
			
			if (create_angle_to_fire) {
				act->turn_to = create_chicken_to_fire(true);
			}
			else {

				act->turn_to = 0;

				going_to_z = 0;
				going_to_x = 1;
				/*std::cout << "creating a new batch of chickens" << std::endl;
				for (int i = 0; i < 5; i++) {
					create_chicken_to_fire(true);
				}

				create_angle_to_fire = true;*/
				current_angle = 0;
			}
		}
		else if (index == 7) {
			if (lower_zap || ready_to_zap) {//lower the platform down
				//std::cout << "setting down" << std::endl;
				dest_x = routines[0][index]->rails[0]->loc.x;
				dest_y = routines[0][index]->rails[0]->loc.y - 6;
				dest_z = routines[0][index]->rails[0]->loc.z;
			}
			else {//raise it back to the def possition
				//std::cout << "setting up" << std::endl;
				dest_x = routines[0][index]->rails[0]->loc.x;
				dest_y = routines[0][index]->rails[0]->loc.y;
				dest_z = routines[0][index]->rails[0]->loc.z;
			}
		}
		else if (index == 8) {
			if (first_plat_cord) {
				dest_x = routines[0][index]->rails[0]->loc.x;
				dest_y = routines[0][index]->rails[0]->loc.y;
				dest_z = routines[0][index]->rails[0]->loc.z;

				act->object->x = dest_x;
				act->object->y = dest_y;
				act->object->z = dest_z;

				first_plat_cord = false;
			}
			else if (ready_to_lower && lowering){//lower the platform down
				dest_x = routines[0][index]->rails[0]->loc.x;
				dest_y = routines[0][index]->rails[0]->loc.y - 3;
				dest_z = routines[0][index]->rails[0]->loc.z;
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
				dest_x = routines[0][index]->rails[0]->loc.x;
				dest_y = routines[0][index]->rails[0]->loc.y;
				dest_z = routines[0][index]->rails[0]->loc.z;
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

	create_angle_to_fire = false;
	play_sound = true;
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> distribution(routines[0][index]->x_min, routines[0][index]->x_max);
	dest_x = distribution(mt);
	distribution = std::uniform_real_distribution<float>(routines[0][index]->z_min, routines[0][index]->z_max);
	dest_z = distribution(mt);

	going_to_x = dest_x;
	going_to_z = dest_z;

	float dot = x1 * dest_x + y1 * dest_z;
	float det = x1 * dest_z - y1 * dest_x;
	int angle = atan2(det, dot);

	angle =atan2(dest_z- cannon_og.z, dest_x - cannon_og.x)*(180 / 3.14159265);

	//std::cout << "created angle " << angle << std::endl;

	chickens_to_make_angles.push(angle);
	chickens_to_make.push(cursed);

	if (angle >= 90 && angle <= 270) {
		angle + 180;
	}

	current_angle = angle;
	return angle;
}

void animation_manager::create_chicken() {
//	std::cout << std::endl;
	//std::cout << "creating a chicken" << std::endl;
	//std::cout << std::endl;

	int dim = 5;
	double radius = dim / 2;

	//current_angle = 90;

	float x = radius * glm::cos(glm::degrees(float(current_angle)));
	float z = radius * glm::sin(glm::degrees(float(current_angle)));
	float y = 6 * 2;

	x += cannon_og.x;
	z += cannon_og.z;

	//std::cout << "angle " << current_angle << std::endl;
	//std::cout << "x " << x << " y " << y << " z " << z << std::endl;

	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, glm::vec3(x, y, z));
	item_info* temp_data = OBJM->spawn_item(CURSE_CHICKEN_T, -1, -1, -1, mat);
	temp_data->x = x;
	temp_data->y = y;
	temp_data->z = z;
	turn_object_into_actor(temp_data, CHICKEN_TRANS2_ROUTINE);

	x_t = x;
	y_t = y;
	z_t = z;

}

void animation_manager::update_actor_id(int id, glm::vec3& loc, glm::vec3& rot, float angle) {
	if (id > 0 && id < actors[0].size()) {
		update_pak update_pac;
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, loc);
		trans = glm::rotate(trans, glm::radians(angle), rot);
		update_pac.x = loc.x;
		update_pac.y = loc.y;
		update_pac.z = loc.z;

		update_pac.x_scale = 1;
		update_pac.y_scale = 1;
		update_pac.z_scale = 1;

		update_pac.buffer_loc = actors[0][id]->object->buffer_loc;
		update_pac.item_id = actors[0][id]->object->item_id;

		OBJM->update_item_matrix(&update_pac, trans);
	}
	else {
		std::cout << "could not updated " << id << ", it was out of bounds of actors" << std::endl;
	}
}

void animation_manager::print_routines() {

	for (int i = 0; i < routines[0].size(); i++) {
		if (routines[0][i] != NULL) {
			if (routines[0][i]->defined) {
				
				switch (routines[0][i]->designation)
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
				std::cout << "min x: " << routines[0][i]->x_min << " max_x: " << routines[0][i]->x_max << std::endl;
				std::cout << "min z: " << routines[0][i]->z_min << " max_z: " << routines[0][i]->z_max << std::endl;

				std::cout << "behavior: " << routines[0][i]->z_min << routines[0][i]->behavior << std::endl;
				std::cout << "flee_player: " << routines[0][i]->z_min << routines[0][i]->flee_player << std::endl;
				std::cout << "min_flee_distance: " << routines[0][i]->z_min << routines[0][i]->min_flee_distance << std::endl;
				std::cout << "return_area: " << routines[0][i]->z_min << routines[0][i]->return_area << std::endl;

			}
			else {
				std::cout << "this routine has not been defined" << std::endl;
			}
		}
		std::cout << std::endl;
	}
}

void animation_manager::update_robots(float* time) {

	for (int i = 0; i < robots.size(); i++) {
		glm::vec3 current_loc = glm::vec3(robots[i]->body->x, robots[i]->body->y, robots[i]->body->z);
		if (!robots[i]->nav_points.empty()) {

		/*	distraction* dist = is_robot_in_range_of_distraction(robots[i]);
			if (dist != NULL) {
				generate_points_for_robot(robots[i], dist);
			}*/
			float speed = robots[i]->move_speed * (*time);

			//int route_index = get_routine_index(robots[i]->routine);

			//std::cout << "updateing " << route_index<<" with behavior "<< routines[0][route_index]->behavior << std::endl;
			//if (routines[0][route_index]->behavior == 1) {
				//std::cout << "updateing " << route_index<<" with behavior "<< routines[0][route_index]->behavior << std::endl;

				//react to the cammera location
			//if (routines[0][route_index]->behavior == 1) {
			//	glm::vec3 cam_loc = cam->get_pos();

			//	if ((diff_btwn_pnt(current_loc.x, cam_loc.x) >= 0 && diff_btwn_pnt(current_loc.x, cam_loc.x) <= 10)
			//		&& (diff_btwn_pnt(current_loc.z, cam_loc.z) >= 0 && diff_btwn_pnt(current_loc.z, cam_loc.z) <= 10)) {

			//		if (determin_direction(current_loc.x, cam_loc.x)) {
			//			actors[0][i]->nav_points[0].x = current_loc.x - 4.0f;
			//		}
			//		else {
			//			actors[0][i]->nav_points[0].x = current_loc.x + 4.0f;
			//		}

			//		if (determin_direction(current_loc.z, cam_loc.z)) {
			//			actors[0][i]->nav_points[0].z = current_loc.z - 4.0f;
			//		}
			//		else {
			//			actors[0][i]->nav_points[0].z = current_loc.z + 4.0f;
			//		}

			//		if (actors[0][i]->cooldown <= 0) {
			//			//std::cout << "playing sound" << std::endl;
			//			sound_system->play_3D_sound(chicken_alarm_call, current_loc);
			//			actors[0][i]->cooldown = actors[0][i]->cooldown_max;
			//		}
			//	}
			//}


			glm::vec3 nav_point = robots[i]->nav_points[0];
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


			robots[i]->body->x = current_loc.x;
			robots[i]->body->y = current_loc.y;
			robots[i]->body->z = current_loc.z;

			update_pak update_pac;

			update_pac.x = current_loc.x;
			update_pac.y = current_loc.y;
			update_pac.z = current_loc.z;

			update_pac.x_scale = 1;
			update_pac.y_scale = 1;
			update_pac.z_scale = 1;

			update_pac.buffer_loc = robots[i]->body->buffer_loc;
			update_pac.item_id = robots[i]->body->item_id;

			OBJM->update_item_matrix(&update_pac);

			
			//if (robots[i]->made_to_start && Stealth != NULL) {
			//	//Stealth->set_debug_stealth_test(current_loc, robots[i]->head_angle, 6);
			//	if (Stealth->is_cam_in_veiw(cam->get_pos(), current_loc, robots[i]->head_angle, 6)) {
			//		std::cout << "player is in range of robot " << i << std::endl;
			//		robots[i]->chaseing_player = true;
			//		generate_points_for_robot(robots[i]);
			//	}
			//}
			//else {
			//	std::cout << "Stealth was null" << std::endl;
			//}

			if (robots[i]->routine != NULL && robots[i]->routine->look_around) {
				update_pac.buffer_loc = robots[i]->head->buffer_loc;
				update_pac.item_id = robots[i]->head->item_id;
				if (robots[i]->alerted) {
					robots[i]->head_angle += robots[i]->turn_speed_fast * (*time);
				}
				else {
					robots[i]->head_angle += robots[i]->turn_speed * (*time);
				}
				if (robots[i]->head_angle > 360) {
					robots[i]->head_angle -= 360;
				}
				//std::cout << "head_angle: " << robots[i]->head_angle << std::endl;
				update_pac.angle = robots[i]->head_angle;
				update_pac.rox_x = 0;
				update_pac.rox_y = 1;
				update_pac.rox_z = 0;
				OBJM->update_item_matrix(&update_pac);
			}

			if (reached_x && reached_z && reached_y) {

				if (!robots[i]->made_to_start) {
					robots[i]->made_to_start = true;
				}

				robots[i]->map_x = robots[i]->dest_x;
				robots[i]->map_z = robots[i]->dest_z;

				if (robots[i]->nav_points.size() == 1) {
					robots[i]->nav_points.pop_back();
				}
				else {
					robots[i]->nav_points.erase(robots[i]->nav_points.begin());
				}
			}

		}
		else {
			if(generate_points_for_robot(robots[i])){
				//if the robot is stationary and need to do other things
			
				/*distraction* dist = is_robot_in_range_of_distraction(robots[i]);
				if (dist != NULL) {
					generate_points_for_robot(robots[i], dist);
				}*/

				if (robots[i]->routine != NULL && robots[i]->routine->look_around) {
					robots[i]->body->x = current_loc.x;
					robots[i]->body->y = current_loc.y;
					robots[i]->body->z = current_loc.z;

					update_pak update_pac;

					update_pac.x = current_loc.x;
					update_pac.y = current_loc.y;
					update_pac.z = current_loc.z;

					update_pac.x_scale = 1;
					update_pac.y_scale = 1;
					update_pac.z_scale = 1;

					update_pac.buffer_loc = robots[i]->head->buffer_loc;
					update_pac.item_id = robots[i]->head->item_id;
					if (robots[i]->alerted) {
						robots[i]->head_angle += robots[i]->turn_speed_fast * (*time);
					}
					else {
						robots[i]->head_angle += robots[i]->turn_speed * (*time);
					}
					if (robots[i]->head_angle > 360) {
						robots[i]->head_angle -= 360;
					}
					//std::cout << "head_angle: " << robots[i]->head_angle << std::endl;
					update_pac.angle = robots[i]->head_angle;
					//update_pac.angle = 180;
					update_pac.rox_x = 0;
					update_pac.rox_y = 1;
					update_pac.rox_z = 0;
					OBJM->update_item_matrix(&update_pac);
				}

				if (!robots[i]->made_to_start) {
					robots[i]->made_to_start = true;
				}

				if (robots[i]->made_to_start && Stealth != NULL) {
					//Stealth->set_debug_stealth_test(current_loc, robots[i]->head_angle, 6);
					if (Stealth->is_cam_in_veiw(cam->get_pos() ,current_loc, robots[i]->head_angle, 6)) {
						std::cout << "player is in range of robot "<<i << std::endl;
						robots[i]->chaseing_player = true;
						generate_points_for_robot(robots[i]);
					}
				}
				else {
					std::cout << "Stealth was null" << std::endl;
				}

			}
		}
	}
}

int animation_manager::create_route(std::vector<int>& x_points, std::vector<int>& z_points, std::string* name, bool stat) {
	int id = robot_routes.size();
	robot_route* temp_route = new robot_route;
	temp_route->name = name;
	temp_route->id = id;
	temp_route->stationary = stat;
	//if (!stat) {
		for (int i = 0; i < x_points.size(); i++) {
			map_pair map_temp;
			map_temp.x = x_points[i];
			map_temp.z = z_points[i];

			temp_route->nav_points.push_back(map_temp);
		}
	//}
	robot_routes.push_back(temp_route);
	return id;
}

void animation_manager::turn_objects_into_actor(item_info* body, item_info* head, int route_id) {
	actor_robot* new_robot = new  actor_robot;
	new_robot->id = robot_id;
	robot_id++;
	new_robot->head = head;
	new_robot->body = body;
	new_robot->routine = NULL;
	if (route_id >= 0 && route_id < robot_routes.size()) {
		new_robot->routine = robot_routes[route_id];
	}
	else {
		std::cout << "route_id(" << route_id << ") does match a rout that has been created, total: " << robot_routes.size() << std::endl;
	}

	//generate_points_for_robot(new_robot);
	robots.push_back(new_robot);
}

bool animation_manager::generate_points_for_robot(actor_robot* new_robot, distraction* dist) {
	//std::cout << "generating points" << std::endl;
	if (new_robot == NULL) {
		return false;
	}

	if (new_robot->chaseing_player) {
		if (Pathing != NULL && cam !=NULL) {
			glm::vec3 pos = cam->get_pos();
			int x = pos.x / 2;
			int z = pos.z / 2;
			if (x < 0) {
				x = 0;
			}
			if (z < 0) {
				z = 0;
			}
			new_robot->nav_points = *Pathing->get_pathing(new_robot->map_x, new_robot->map_z, z, x);
			new_robot->index = 0;
			new_robot->dest_x = z;
			new_robot->dest_z = x;
			new_robot->map_x = z;
			new_robot->map_z = x;
		}
	}

	dist = is_robot_in_range_of_distraction(new_robot);
	if (dist == NULL) {
		int index = new_robot->index;
		index++;

		if (new_robot->routine->stationary) {
			if (Pathing != NULL && new_robot->routine != NULL) {
				robot_route* routine = new_robot->routine;
				//	std::cout << "updating stationary points 1" << std::endl;
				if (!routine->nav_points.empty() &&
					(new_robot->map_x != new_robot->routine->nav_points[0].x) &&
					(new_robot->map_z != new_robot->routine->nav_points[0].z)) {
					//std::cout << "stationary robot not at his pos" << std::endl;
					robot_route* routine = new_robot->routine;
					int x = routine->nav_points[0].x;
					int z = routine->nav_points[0].z;
					//std::cout << "updating stationary points 3" << std::endl;
					new_robot->nav_points = *Pathing->get_pathing(new_robot->map_x, new_robot->map_z, x, z);
					new_robot->dest_x = x;
					new_robot->dest_z = z;
				}
				else {
					//	std::cout << "no nav points" << std::endl;
				}
			}
			else {
				std::cout << "new_robot->routine == NULL" << std::endl;
			}
			return true;
		}


		if (new_robot->routine != NULL) {
			//std::cout << "index "<<index<<" size "<< new_robot->routine->nav_points.size() << std::endl;
			robot_route* routine = new_robot->routine;
			if (index >= routine->nav_points.size()) {
				index = 0;
			}

			int x = routine->nav_points[index].x;
			int z = routine->nav_points[index].z;

			if (Pathing != NULL) {
				new_robot->nav_points = *Pathing->get_pathing(new_robot->map_x, new_robot->map_z, x, z);
				//glm::vec3 temp_point(x * 2, new_robot->body->y, z * 2);
				//new_robot->nav_points.push_back(temp_point);
				new_robot->index = index;
				new_robot->dest_x = x;
				new_robot->dest_z = z;

			}
			else {
				std::cout << "pathing is null" << std::endl;
			}
		}
		else {// no routine

		}


	}
	else {
		//go to distraction
		int x = dist->x;
		int z = dist->z;

		new_robot->nav_points = *Pathing->get_pathing(new_robot->map_x, new_robot->map_z, x, z);
		new_robot->index = 0;
		new_robot->dest_x = x;
		new_robot->dest_z = z;
		new_robot->map_x = x;
		new_robot->map_z = z;
	}
	return false;
}

float animation_manager::distance(float x1, float y1, float x2, float y2){
	return sqrt(pow(x2 - x1, 2) +
		pow(y2 - y1, 2) * 1.0);
}

distraction* animation_manager::is_robot_in_range_of_distraction(actor_robot* robot) {
	float dist;
	for (int i = 0; i < distractions.size(); i++) {
		dist = distance(distractions[i]->x, distractions[i]->z, robot->body->x / 2, robot->body->z / 2);
		//if (dist < distractions[i]->radius) {
			std::cout << "robot "<< robot->id<<" is in range" << std::endl;
			return distractions[i];
		//}
	}
	return NULL;
}

void animation_manager::drop_trap(item_info* trap) {

	int map_x = trap->x/2;
	int map_z = trap->z/2;
	std::cout << "dropping a trap map_x: " << map_x << " map_z " << map_z << std::endl;
	float time_for_trap = 5;

	distraction* temp_dist = new distraction;

	temp_dist->x = map_x;
	temp_dist->z = map_z;
	temp_dist->pick_up_return = false;
	temp_dist->max_time = time_for_trap;
	temp_dist->time_left = time_for_trap;
	distractions.push_back(temp_dist);
}

void animation_manager::update_distractions(float* time) {

	for (int i = 0; i < distractions.size(); i++) {
		distractions[i]->time_left -= *time;
		if (distractions[i]->time_left <= 0) {
			std::cout << "trap ran out of juice" << std::endl;
			if (distractions.size() == 1) {
				delete distractions[i];
				distractions.pop_back();
			}
			else {
				distraction* temp_dist = distractions[i];
				distractions[i] = distractions[distractions.size() - 1];
				delete temp_dist;
				distractions.pop_back();
				i--;
				if (i < 0) {
					i = 0;
				}
			}
		}
	}
}

item_info* animation_manager::activate_item() {
	float pick_up_range = 7;
	glm::vec3 pos = cam->get_pos();
	for (int i = 0; i < activatible_items.size(); i++) {
		if (pick_up_range >distance(pos.x, pos.z,
			activatible_items[i]->x, activatible_items[i]->z)) {

			std::cout << "active item found" << std::endl;
			if (activatible_items[i]->activatible) {
				if (activatible_items[i]->computer) {
					std::cout << "it is a computer" << std::endl;
					comp_on = !comp_on;
				}
				else {
					std::cout << "it is a leaver" << std::endl;
					if (activatible_items[i]->link1 != NULL) {
						activatible_items[i]->link1->activated = true;
					}
					if (activatible_items[i]->link2 != NULL) {
						activatible_items[i]->link2->activated = true;
					}
				}
			}
			else if (activatible_items[i]->pickup) {
				std::cout << "it is a pickup" << std::endl;
			}
			break;
		}
		else {
			std::cout << "dist " << distance(pos.x, pos.z,
				activatible_items[i]->x, activatible_items[i]->z) << std::endl;
		}
	}
	return NULL;
}



void animation_manager::create_interactible(item_info* p, bool activatible, bool comp, door_actor* link1, door_actor* link2 ) {
	interactibles* temp = new interactibles;
	if (p == NULL) {
		delete temp;
		return;
	}
	temp->part = p;
	temp->link1 = link1;
	temp->link2 = link2;
	temp->x = p->x;
	temp->z = p->z;
	if (activatible) {
		temp->activatible = activatible;
		temp->pickup = false;
		temp->computer = comp;
	}
	else {
		temp->activatible = false;
		temp->computer = false;
		temp->pickup = true;
	}
	activatible_items.push_back(temp);
}

void animation_manager::clear_all_object() {

	distractions.clear();
	activatible_items.clear();
	delete routines;
	delete actors_doors;
	delete actors;
	openIDs.clear();

	actors = new std::vector<actor*>();
	actors_doors = new std::vector<door_actor*>();
	routines = new std::vector<routine*>();

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

	play_sound = false;
	create_angle_to_fire = false;
	cannon_og = glm::vec3(-1, -1, -1);
	server = true;

	//physics setters

	update_physics = false;
	init_physics_ = false;

	//robot setters
	robot_id = 0;

	routine* def_routine;

	for (int i = 0; i < routine_total_predefined; i++) {
		def_routine = new routine;
		routines[0].push_back(def_routine);
	}
}

void animation_manager::open_doors() {
	doors_open = true;
	for (int i = 0; i < actors_doors[0].size(); i++) {
		actors_doors[0][i]->activated = true;
	}
}

void animation_manager::close_doors() {
	open_doors();
	doors_open = false;
}