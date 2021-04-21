#include "city.h"


city::city() {
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);
	Time = NULL;

	city_info = NULL;
	layout = NULL;
	AM = NULL;
	cube_matrices = NULL;//contains all the cubes mats
	cube_shader = NULL;
	
	//debug information
	draw_wall_c = true;
	draw_wall = true;
	draw_path_cubes = true;
	draw_light_posts = true;
	draw_sidewalk = true;

	server = true;
	online = false;
}

city::~city(){
	if (city_info != NULL) {
		delete city_info;
	}
	if (cube_matrices != NULL) {
		delete cube_matrices;
	}
	if (cube_shader != NULL) {
		delete cube_shader;
	}
	if (pathing != NULL) {
		delete pathing;
	}
}

void city::draw() {

}

void city::update() {

}

void city::add_object_to_cell(item_info* item_data, rending_cell** cells, int xpos, int ypos,bool fromcell) {
	if (city_info == NULL || cells == NULL) {
		std::cout << "can not add to cell, city info never created" << std::endl;
		return;
	}

	if (fromcell) {
		cells[xpos][ypos].obj_in_cell.push_back(item_data);
		return;
	}

	if (item_data->x == -1 ||
		item_data->y == -1 ||
		item_data->z == -1) {
		std::cout << "object has -1 cords" << std::endl;
		return;
	}
	

	//brute force place the block
	for (int i = 0; i < city_info->get_height(); i++) {
		for (int h = 0; h < city_info->get_width(); h++) {
			if ((xpos >= cells[i][h].start_x && xpos <= cells[i][h].end_x) &&
				(ypos >= cells[i][h].start_z && ypos <= cells[i][h].end_z)) {
				cells[i][h].obj_in_cell.push_back(item_data);
				return;
			}
		}
	}
	std::cout << "object could not be placed in cell x: " << xpos << " z: " << ypos << std::endl;
}

void city::init(object_manger* OBJM, animation_manager* an) {
	std::cout << "creating city" << std::endl;
	std::cout << "creating city info" << std::endl;

	/*if (online && !server) {
		init_client_world(OBJM, an);
			return;
	}*/

	if (city_info == NULL) {
		city_info = new city_gen();
	}

	city_info->set_time(Time);
	city_info->set_projection(projection);
	city_info->init();
	height_map = city_info->get_height_map();
	AM = an;

	layout = city_info->get_layout();
	int** layout_expanded = city_info->get_expanded_layout();

	int key = city_info->get_expandion_key();
	x_width = city_info->get_height() * key;
	z_width = city_info->get_width() * key;

	rending_cell** cells;

	cells = new rending_cell * [city_info->get_height()];
	for (int i = 0; i < city_info->get_height(); i++) {
		cells[i] = new rending_cell[city_info->get_width()];
	}

	for (int i = 0; i < city_info->get_height(); i++) {
		for (int h = 0; h < city_info->get_width(); h++) {
			//for (int x = 0; x < key; x++) {
			//	for (int y = 0; y < key; y++) {
			int x = key - 1;
			int y = key - 1;
			cells[i][h].start_x = (i * key);
			cells[i][h].start_z = (h * key);
			cells[i][h].end_x = (i * key) + x;
			cells[i][h].end_z = (h * key) + y;
			//	}
			//}

		}
	}

	//x_width = 9;// ROW;//rows
	//z_width = 10;//COL;//collums


	OBJM->set_block_size(0, 0, x_width, z_width, key);
	int chicken_x_s = -1;
	int chicken_z_s = -1;

	int chicken_x_e = -1;
	int chicken_z_e = -1;


	//hard coded for now
	//134,150 || hig 188,188
	chicken_x_s = 134;
	chicken_z_s = 150;

	chicken_x_e = 188;
	chicken_z_e = 188;

	AM->define_routine(DEFF_ERROR_ROUTINE, -1, -1, -1, -1);
	AM->define_routine(CHICKEN_ROUTINE, chicken_x_s, chicken_z_s, chicken_x_e, chicken_z_e);
	AM->define_routine(CHICKEN_TRANS2_ROUTINE, chicken_x_s, chicken_z_s, chicken_x_e , chicken_z_e );
	
	int low_x = 10000000;
	int low_z = 10000000;
	int hig_x = 0;
	int hig_z = 0;

	int num_chickens = 0;

	std::vector< rail_section*> rails = city_info->get_rails();
	item_info* tempdata = NULL;
	item_info* cart = NULL;
	item_info* zap_t = NULL;
	item_info* zap_s = NULL;
	item_info* platform = NULL;
	item_info* cannon = NULL;
	std::vector<rail_check_point*> temp_rails;

	glm::vec3 platform_point = glm::vec3(-1,-1,-1);
	glm::vec3 platform_point_ob = glm::vec3(-1, -1, -1);

	glm::vec3 zap_o_tron = glm::vec3(-1, -1, -1);

	for (int i = 0; i < rails.size(); i++) {
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, rails[i]->loc);
		trans = glm::rotate(trans, glm::radians(rails[i]->angle), rails[i]->rot);

		if (i == 0) {
			cart = OBJM->spawn_item(SKYTRACK_CART, -1, -1, -1, trans);
			cart->x_rot = 0;
			cart->y_rot = 1;
			cart->z_rot = 0;
			cart->angle = 0;
			cart->x = rails[i]->loc.x;
			cart->y = rails[i]->loc.y;
			cart->z = rails[i]->loc.z;
		}

		if (i == rails.size() - 1) {
			glm::vec3 temp_loc = rails[i]->loc;
			glm::mat4 temp_trans = glm::mat4(1.0f);

			temp_loc.z -= 6;
			platform_point = temp_loc;
			platform_point.y += 3.8;
			
			temp_loc.y += 8;
			zap_o_tron = temp_loc;
			temp_trans = glm::translate(temp_trans, temp_loc);
			zap_t = OBJM->spawn_item(ZAP_TOWER_T, -1, -1, -1, temp_trans);
			zap_t->x_rot =0;
			zap_t->y_rot = 1;
			zap_t->z_rot =0;
			zap_t->angle = 0;
			zap_t->x = temp_loc.x;
			zap_t->y = temp_loc.y;
			zap_t->z = temp_loc.z;

			zap_s = OBJM->spawn_item(ZAP_SPHERE_T, -1, -1, -1, temp_trans);
			zap_s->x_rot = 0;
			zap_s->y_rot = 1;
			zap_s->z_rot = 0;
			zap_s->angle = 0;
			zap_s->x = temp_loc.x;
			zap_s->y = temp_loc.y;
			zap_s->z = temp_loc.z;
			temp_loc.y -= 8;
			temp_loc.x += 15;

			temp_trans = glm::mat4(1.0f);
			temp_trans = glm::translate(temp_trans, temp_loc);
			temp_trans = glm::rotate(temp_trans, glm::radians(180.0f), rails[i]->rot);

			tempdata = OBJM->spawn_item(CANNON_FRAME_T, -1, -1, -1, temp_trans);

			tempdata->x_rot = rails[i]->rot.x;
			tempdata->y_rot = rails[i]->rot.y;
			tempdata->z_rot = rails[i]->rot.z;
			tempdata->angle = 180.0f;
			tempdata->x = temp_loc.x;
			tempdata->y = temp_loc.y;
			tempdata->z = temp_loc.z;

			platform_point_ob = temp_loc;
			platform = OBJM->spawn_item(CANNON_PLATFORM_T, -1, -1, -1, temp_trans);

			platform->x_rot = rails[i]->rot.x;
			platform->y_rot = rails[i]->rot.y;
			platform->z_rot = rails[i]->rot.z;
			platform->angle = 180.0f;
			platform->x = temp_loc.x;
			platform->y = temp_loc.y;
			platform->z = temp_loc.z;

			cannon = OBJM->spawn_item(CANNON_T, -1, -1, -1, temp_trans);

			cannon->x_rot = rails[i]->rot.x;
			cannon->y_rot = rails[i]->rot.y;
			cannon->z_rot = rails[i]->rot.z;
			cannon->angle = 180.0f;
			cannon->x = temp_loc.x;
			cannon->y = temp_loc.y;
			cannon->z = temp_loc.z;
		}


		if (rails[i]->type == 1) {
			tempdata = OBJM->spawn_item(SKYTRACK_C_T, -1, -1, -1, trans);
		}
		else {
			tempdata = OBJM->spawn_item(SKYTRACK_S_T, -1, -1, -1, trans);
		}
		tempdata->x_rot = rails[i]->rot.x;
		tempdata->y_rot = rails[i]->rot.y;
		tempdata->z_rot = rails[i]->rot.z;
		tempdata->angle = rails[i]->angle;
		tempdata->x = rails[i]->loc.x;
		tempdata->y = rails[i]->loc.y;
		tempdata->z = rails[i]->loc.z;

		rail_check_point* temp = new rail_check_point;
		temp->loc = rails[i]->loc;
		temp->rail_type = rails[i]->type;

		temp_rails.push_back(temp);
	}

	//create the routines based off of the rails
	AM->define_routine(RAIL_ROUTINE, temp_rails);
	AM->define_routine(CHICKEN_TRANS1_ROUTINE, platform_point);
	AM->define_routine(ZAP_TOWER_ROUTINE, zap_o_tron);
	AM->define_routine(ZAP_SPHERE_ROUTINE, zap_o_tron);

	AM->define_routine(CANNON_ROUTINE, platform_point_ob);


	platform_point_ob.x -= 30;
	AM->define_routine(CANNON_PLATFORM_ROUTINE, platform_point_ob);


	//define the objects with routines
	AM->turn_object_into_actor(cart, RAIL_ROUTINE);
	AM->turn_object_into_actor(platform, CANNON_PLATFORM_ROUTINE);
	AM->turn_object_into_actor(zap_t, ZAP_TOWER_ROUTINE);
	AM->turn_object_into_actor(zap_s, ZAP_SPHERE_ROUTINE);

	AM->turn_object_into_actor(cannon, CANNON_ROUTINE);

	std::cout << "spawning in objects" << std::endl;
	bool  one_time = false;
	for (int i = 0; i < x_width; i++) {
		for (int h = 0; h < z_width; h++) {
			item_info* tempdata;
			float angle = 0;
			if (layout_expanded[i][h] == 1) {//generate path finding cubes
				glm::mat4 temp = glm::mat4(1.0f);

				temp = glm::translate(temp, glm::vec3(h * 2, 0, i * 2));
				tempdata = OBJM->spawn_item(CUBE_T, h * 2, 0, i * 2, temp);
				add_object_to_cell(tempdata, cells,i,h);
				//generated_mats_debug_cubes.push_back(temp);
			}
			else if (layout_expanded[i][h] == 3) {//for a wall
				//std::cout << "wall start found" << std::endl;
				glm::mat4 temp = glm::mat4(1.0f);

				temp = glm::translate(temp, glm::vec3(h * 2, 2, i * 2));
				if (i - 1 >= 0 && layout_expanded[i - 1][h] != 0) {//if the topspot is a rode
					temp = glm::rotate(temp, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 270;
				}
				else if (i + 1 < x_width && layout_expanded[i + 1][h] != 0) {
					temp = glm::rotate(temp, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 270;
				}
				else {
					temp = glm::rotate(temp, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 180;
				}
				tempdata = OBJM->spawn_item(WALL_T, -1, -1, -1, temp);
				tempdata->x_rot = 0;
				tempdata->y_rot = 1;
				tempdata->z_rot = 0;
				tempdata->angle = angle;
				tempdata->x = h * 2;
				tempdata->y = 2;
				tempdata->z = i * 2;
				add_object_to_cell(tempdata,cells,i,h);

				//generated_mats_wall.push_back(temp);
			}
			else if (layout_expanded[i][h] == 4 || (layout_expanded[i][h] >= 14 && layout_expanded[i][h] <= 16)) {//for a corner

				//std::cout << "wall_c start found" << std::endl;
				glm::mat4 temp = glm::mat4(1.0f);

				temp = glm::translate(temp, glm::vec3(h * 2, 2, i * 2));

				switch (layout_expanded[i][h]) {
				case 14:
					temp = glm::rotate(temp, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 90;
					break;
				case 15:
					temp = glm::rotate(temp, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 180;

					break;
				case 16:
					temp = glm::rotate(temp, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 270;

					break;
				}
				tempdata = OBJM->spawn_item(WALL_C_T, -1, -1, -1, temp);
				tempdata->x_rot = 0;
				tempdata->y_rot = 1;
				tempdata->z_rot = 0;
				tempdata->angle = angle;
				tempdata->x = h * 2;
				tempdata->y = 2;
				tempdata->z = i * 2;
				add_object_to_cell(tempdata,cells,i,h);

				//generated_mats_wall_c.push_back(temp);
			}
			else if (layout_expanded[i][h] == 5 || (layout_expanded[i][h] >= 11 && layout_expanded[i][h] <= 13)) {//lights posts
				glm::mat4 temp = glm::mat4(1.0f);
				temp = glm::translate(temp, glm::vec3(h * 2, 4, i * 2));

				switch (layout_expanded[i][h]) {
				case 11:
					temp = glm::rotate(temp, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 90;
					break;
				case 12:
					temp = glm::rotate(temp, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 180;
					break;
				case 13:
					temp = glm::rotate(temp, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 270;
					break;
				}

				tempdata = OBJM->spawn_item(LIGHT_POST_T, -1, -1, -1, temp);
				tempdata->x_rot = 0;
				tempdata->y_rot = 1;
				tempdata->z_rot = 0;
				tempdata->angle = angle;
				tempdata->x = h * 2;
				tempdata->y = 4;
				tempdata->z = i * 2;
				add_object_to_cell(tempdata,cells,i,h);

				//generated_mats_lightposts.push_back(temp);

			}
			else if (layout_expanded[i][h] >= 6 && layout_expanded[i][h] <= 10) {//placing sidewalks

				glm::mat4 trans = glm::mat4(1.0f);

				int x = 0;
				int y = 2;
				int z = 0;

				std::cout << layout_expanded[i][h] << " ";;

				switch (layout_expanded[i][h]) {
				case 10:
				case 6://sidewalk top
					trans = glm::translate(trans, glm::vec3((h * 2) + 14, 2, i * 2));
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 180;
					x = (h * 2) + 14;
					z = (i * 2);
					break;
				case 7://sidewalk bottom
					trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2) + 14));
					//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					x = (h * 2);
					z = (i * 2) + 14;
					break;
				case 8://sidewalk left
					trans = glm::translate(trans, glm::vec3((h * 2), 2, i * 2));
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					x = (h * 2);
					z = (i * 2);
					angle = 270;
					break;
				case 9://sidewalk right
					trans = glm::translate(trans, glm::vec3((h * 2) + 14, 2, (i * 2) + 14));
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					angle = 90;
					x = (h * 2) + 14;
					z = (i * 2) + 14;
					break;
				default:
					break;
				}
				if (layout_expanded[i][h] == 10) {
					tempdata = OBJM->spawn_item(SIDESTREET_T, -1, -1, -1, trans);

					//generated_mats_sidestreets.push_back(trans);
				}
				else {
					tempdata = OBJM->spawn_item(SIDEWALK_T, -1, -1, -1, trans);
					//generated_mats_sidewalk.push_back(trans);
				}
				tempdata->x_rot = 0;
				tempdata->y_rot = 1;
				tempdata->z_rot = 0;
				tempdata->angle = angle;
				tempdata->x = x;
				tempdata->y = y;
				tempdata->z = z;
				add_object_to_cell(tempdata,cells,i,h);

			}
			else if (layout_expanded[i][h] == 17) {

				if (h * 2 < low_x) {
					low_x = h * 2;
				}
				if (h * 2 > hig_x) {
					hig_x = h * 2;
				}

				if (i * 2 < low_z) {
					low_z = i * 2;
				}
				if (i * 2 > hig_z) {
					hig_z = i * 2;
				}

				num_chickens++;

				glm::mat4 trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((h * 2), 4, (i * 2)));
				tempdata = OBJM->spawn_item(CHICKEN_T, -1, -1, -1, trans);
				tempdata->x_rot = 0;
				tempdata->y_rot = 1;
				tempdata->z_rot = 0;
				tempdata->angle = angle;
				tempdata->x = (h * 2);
				tempdata->y = 4;
				tempdata->z = (i * 2);
				AM->turn_object_into_actor(tempdata, CHICKEN_ROUTINE);
				//spawn a block underneath them
				trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2)));
				tempdata = OBJM->spawn_item(CUBE_T, -1, -1, -1, trans);
				add_object_to_cell(tempdata,cells,i,h);

			}
			else if (layout_expanded[i][h] == 18) {
				//std::cout << h << " " << i << std::endl;
				glm::mat4 trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2)));
				tempdata = OBJM->spawn_item(CUBE_T, -1, -1, -1, trans);
				add_object_to_cell(tempdata,cells,i,h);

			}
			else if (19 <= layout_expanded[i][h] && layout_expanded[i][h] <= 22) {
				//std::cout << h << " " << i << std::endl;
				glm::mat4 trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2)));

				switch (layout_expanded[i][h]) {
				case 20:
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 21:
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 22:
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				}

				tempdata = OBJM->spawn_item(WALL_T, -1, -1, -1, trans);
				add_object_to_cell(tempdata,cells,i,h);

			}
			else if (24 <= layout_expanded[i][h] && layout_expanded[i][h] <= 27) {
				//std::cout << h << " " << i << std::endl;
				glm::mat4 trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2)));

				switch (layout_expanded[i][h]) {
				case 25:
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 26:
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 27:
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				}

				tempdata = OBJM->spawn_item(WALL_D_T, -1, -1, -1, trans);
				add_object_to_cell(tempdata,cells,i,h);

			}
			else if (28 <= layout_expanded[i][h] && layout_expanded[i][h] <= 31) {
				//std::cout << h << " " << i << std::endl;
				glm::mat4 trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2)));

				switch (layout_expanded[i][h]) {
				case 29:
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 30:
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 31:
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				}

				tempdata = OBJM->spawn_item(WALL_LA_T, -1, -1, -1, trans);
				add_object_to_cell(tempdata,cells,i,h);

			}
			else if (32 <= layout_expanded[i][h] && layout_expanded[i][h] <= 35) {
				//std::cout << h << " " << i << std::endl;
				glm::mat4 trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2)));

				switch (layout_expanded[i][h]) {
				case 33:
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 34:
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 35:
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				}

				tempdata = OBJM->spawn_item(FLOOR_LA_T, -1, -1, -1, trans);
				add_object_to_cell(tempdata,cells,i,h);

				//std::cout << "creating a floor to draw" << std::endl;
			}
			else if (36 <= layout_expanded[i][h] && layout_expanded[i][h] <= 39) {
				//std::cout << h << " " << i << std::endl;
				glm::mat4 trans = glm::mat4(1.0f);
				trans = glm::translate(trans, glm::vec3(((h - 2) * 2), 10, (i * 2)));

				switch (layout_expanded[i][h]) {
				case 37:
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 38:
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 39:
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				}

				tempdata = OBJM->spawn_item(WALL_CLOCK_ANG_T, -1, -1, -1, trans);
				//std::cout << "creating a floor to draw" << std::endl;
				add_object_to_cell(tempdata,cells,i,h);

			}
			else if (40 <= layout_expanded[i][h] && layout_expanded[i][h] <= 43) {
				//std::cout << h << " " << i << std::endl;
				glm::mat4 trans = glm::mat4(1.0f);
				int x = (h * 2);
				int y = 5;
				int z = (i * 2) - 1;
				trans = glm::translate(trans, glm::vec3(x, y, z));

				switch (layout_expanded[i][h]) {
				case 41:
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 42:
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 43:
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				}

				tempdata = OBJM->spawn_item(LOADING_DOOR_T, x, y, z, trans);
				tempdata->x = x;
				tempdata->y = y;
				tempdata->z = z;
				if (!one_time) {
					AM->turn_object_into_door(tempdata, LOADING_DOOR_ROUTINE, 3);
					one_time = true;
				}
				else {
					AM->turn_object_into_door(tempdata, LOADING_DOOR_ROUTINE, 2);
					one_time = false;
				}
				add_object_to_cell(tempdata,cells,i,h);

				std::cout << "creating a loading door" << std::endl;
			}
		}
	}

	//spwning in items that does not affect path finding
	cell_data** cell_info = city_info->get_cell_info();

	glm::mat4 trans = glm::mat4(1.0f);

	if (cell_info != NULL) {
		item_gen_info* wall_obj;
		for (int i = 0; i < city_info->get_height(); i++) {
			for (int h = 0; h < city_info->get_width(); h++) {

				//wall objects
				for (int q = 0; q < cell_info[i][h].items_on_wall.size(); q++) {
					wall_obj = cell_info[i][h].items_on_wall[q];

					if (wall_obj->roof) {

						int x = (h * key) * 2;
						x += wall_obj->x_cube_offset * 2;
						int y = wall_obj->floor * (7 * 2);
						if (wall_obj->floor > 1) {//correcting for the change in floor height
							wall_obj->y_cube_offset -= (wall_obj->total_floors-1);
						}
						y += wall_obj->y_cube_offset * 2;
						int z = (i * key) * 2;
						z += wall_obj->z_cube_offset * 2;

						trans = glm::mat4(1.0f);
						trans = glm::translate(trans, glm::vec3(x, y, z));
						if (wall_obj->angle != 0) {
							trans = glm::rotate(trans, glm::radians(wall_obj->angle), glm::vec3(0.0, 1.0, 0.0));
						}
						if (wall_obj->roof_value == 1) {
							tempdata = OBJM->spawn_item(SLANTED_ROOF_T, x, y, z, trans);
							add_object_to_cell(tempdata, cells, i, h, true);
						}
						else if (wall_obj->roof_value == 2) {
							tempdata = OBJM->spawn_item(ROOF_FILL_T, x, y, z, trans);
							add_object_to_cell(tempdata, cells, i, h, true);
						}
						else if (wall_obj->roof_value == 3) {
							//std::cout << "spawning a gneric floor" << std::endl;
							tempdata = OBJM->spawn_item(GENERIC_FLOOR_T, x, y, z, trans);
							add_object_to_cell(tempdata, cells, i, h, true);
						}

					}
					else {
						int x = (h * key) * 2;
						x += wall_obj->x_cube_offset * 2;
						int y = wall_obj->floor * (7 * 2);
						if (wall_obj->floor > 1) {
							wall_obj->y_cube_offset -= 1;
							if (wall_obj->floor > 2) {
								wall_obj->y_cube_offset -= 1;
							}
							if (wall_obj->floor > 3) {
								wall_obj->y_cube_offset -= 1;
							}
						}
						y += wall_obj->y_cube_offset * 2;
						int z = (i * key) * 2;
						z += wall_obj->z_cube_offset * 2;

						trans = glm::mat4(1.0f);
						trans = glm::translate(trans, glm::vec3(x, y, z));
						if (wall_obj->angle != 0) {
							trans = glm::rotate(trans, glm::radians(wall_obj->angle), glm::vec3(0.0, 1.0, 0.0));
						}

						switch (wall_obj->expanded_layout_value)
						{
						case 4:
							tempdata = OBJM->spawn_item(WALL_C_T, x, y, z, trans);
							add_object_to_cell(tempdata, cells, i, h, true);
							break;
						case 15:
							tempdata = OBJM->spawn_item(WALL_C_T, x, y, z, trans);
							add_object_to_cell(tempdata, cells, i, h, true);
							break;
						case 16:
							tempdata = OBJM->spawn_item(WALL_C_T, x, y, z, trans);
							add_object_to_cell(tempdata, cells, i, h, true);
							break;
						case 22:
							tempdata = OBJM->spawn_item(WALL_T, x, y, z, trans);
							add_object_to_cell(tempdata, cells, i, h, true);
							break;
						default:
							std::cout << "does not reconize: " << wall_obj->expanded_layout_value << std::endl;
							break;
						}
					}
				}
			}
		}
	}
	else {
		std::cout << "can not spawn in roofs because cells are null" << std::endl;
	}

	trans = glm::mat4(1.0f);
	
	trans = glm::translate(trans, glm::vec3((2), 6, (2)));


	//test items
	tempdata =OBJM->spawn_item(LEAVER_BOX_T, 1, 6, 1, trans);
	//add_object_to_cell(tempdata,cells,i,h);

	tempdata = OBJM->spawn_item(LEAVER_T, 1, 6, 1, trans);

	//add_object_to_cell(tempdata,cells,i,h);
	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3((4), 6, (2)));
	tempdata = OBJM->spawn_item(TABLE_T, 1, 6, 1, trans);

	//add_object_to_cell(tempdata,cells,i,h);
	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3((6), 6, (2)));
	tempdata = OBJM->spawn_item(COMPUTER_T, 1, 6, 1, trans);
	//add_object_to_cell(tempdata,cells,i,h);

	//test for the pathing area for the chicken routine
	//std::cout << "low  " << low_x << "," << low_z << " || hig " << hig_x << "," << hig_z << std::endl;
	std::cout << "chicken number " <<num_chickens<< std::endl;

	std::cout<< std::endl;

	std::cout << "done "<< std::endl;
	std::cout << "creating pathfinding data" << std::endl;


	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3((10), 4, (10)));
	tempdata = OBJM->spawn_item(CHICKEN_T, -1, -1, -1, trans);
	tempdata->x_rot = 0;
	tempdata->y_rot = 1;
	tempdata->z_rot = 0;
	tempdata->angle = 0;
	tempdata->x = 10;
	tempdata->y = 4;
	tempdata->z = 10;
	AM->turn_object_into_actor(tempdata, DEFF_ERROR_ROUTINE);

	trans = glm::translate(trans, glm::vec3((10), 4, (10)));
	tempdata = OBJM->spawn_item(CURSE_CHICKEN_T, -1, -1, -1, trans);
	tempdata->x_rot = 0;
	tempdata->y_rot = 1;
	tempdata->z_rot = 0;
	tempdata->angle = 0;
	tempdata->x = 10;
	tempdata->y = 4;
	tempdata->z = 10;
	//add_object_to_cell(tempdata,cells,i,h);

	cube_amount = x_width * z_width;

	pathing = new path_finding();
	pathing->set_x_width(x_width);
	pathing->set_z_width(z_width);
	pathing->set_cube_amount(cube_amount);
	pathing->set_expanded_layout(layout_expanded);
	pathing->set_key_for_passing_through(city_info->get_pas_key(), city_info->get_unq_obj_cnt());
	pathing->init();
	std::cout << "done" << std::endl;

	OBJM->set_city_layout_cells(cells, city_info->get_height(), city_info->get_width());

	/*OBJM->create_height_map_debig(city_info->get_height_map(),
		city_info->get_height()* key, city_info->get_width()* key,
		city_info->get_def_height());*/

	//city_info->print_height_map();
	if (cube_shader == NULL) {
		cube_shader = new Shader("asteroids.vs", "asteroids.fs");
	}
	else {
		std::cout << "using premade shader for the cubes" << std::endl;
	}

	std::cout << "done creating city" << std::endl;


	check();
	//while (true);
}

void city::init_client_world(object_manger* OBJM, animation_manager* an) {
	std::cout << "creating city (client side)" << std::endl;
	std::cout << "creating city info" << std::endl;

	if (city_info == NULL) {
		city_info = new city_gen();
	}

	city_info->set_time(Time);
	city_info->set_projection(projection);
	city_info->init();


	layout = city_info->get_layout();
	int** layout_expanded = city_info->get_expanded_layout();

	//x_width = 9;// ROW;//rows
	//z_width = 10;//COL;//collums
	int key = city_info->get_expandion_key();
	x_width = city_info->get_height() * key;
	z_width = city_info->get_width() * key;


	int chicken_x_s = -1;
	int chicken_z_s = -1;

	int chicken_x_e = -1;
	int chicken_z_e = -1;


	//hard coded for now
	//134,150 || hig 188,188
	chicken_x_s = 134;
	chicken_z_s = 150;

	chicken_x_e = 188;
	chicken_z_e = 188;


	int low_x = 10000000;
	int low_z = 10000000;
	int hig_x = 0;
	int hig_z = 0;

	int num_chickens = 0;

	std::vector< rail_section*> rails = city_info->get_rails();
	item_info* tempdata = NULL;
	item_info* cart = NULL;
	item_info* zap_t = NULL;
	item_info* zap_s = NULL;
	item_info* platform = NULL;
	item_info* cannon = NULL;
	std::vector<rail_check_point*> temp_rails;

	glm::vec3 platform_point = glm::vec3(-1, -1, -1);
	glm::vec3 platform_point_ob = glm::vec3(-1, -1, -1);

	glm::vec3 zap_o_tron = glm::vec3(-1, -1, -1);

	for (int i = 0; i < rails.size(); i++) {


		if (i == rails.size() - 1) {
			glm::vec3 temp_loc = rails[i]->loc;

			temp_loc.z -= 6;
			platform_point = temp_loc;
			platform_point.y += 3.8;

			temp_loc.y += 8;
			zap_o_tron = temp_loc;
			
			temp_loc.y -= 8;
			temp_loc.x += 15;

			platform_point_ob = temp_loc;
		}


	

		rail_check_point* temp = new rail_check_point;
		temp->loc = rails[i]->loc;
		temp->rail_type = rails[i]->type;

		temp_rails.push_back(temp);
	}

	//create the routines based off of the rails

	platform_point_ob.x -= 30;
	
	std::cout << std::endl;

	std::cout << "done " << std::endl;
	std::cout << "creating pathfinding data" << std::endl;

	cube_amount = x_width * z_width;


	std::cout << "done" << std::endl;
	std::cout << "creating the model buffer" << std::endl;

	if (cube_shader == NULL) {
		cube_shader = new Shader("asteroids.vs", "asteroids.fs");
	}
	else {
		std::cout << "using premade shader for the cubes" << std::endl;
	}

	std::cout << "done creating city" << std::endl;

	check();
	//while (true);
}

void city::check() {
	std::cout << "checking to maksure that all vars were inited" << std::endl;

	bool error = false;
	if (view == glm::mat4(1.0f)) {
		std::cout << "view was never inited" << std::endl;
	}
	if (projection == glm::mat4(1.0f)) {
		std::cout << "projection was never inited" << std::endl;
	}
	if (Time == NULL) {
		std::cout << "Time was never inited" << std::endl;
	}
	if (city_info == NULL) {
		std::cout << "city_info was never inited" << std::endl;
	}
	if (layout == NULL) {
		std::cout << "layout was never inited" << std::endl;
	}
	if (cube_amount == 0) {
		std::cout << "cube_amount was never inited" << std::endl;
	}
	if (cube_matrices == NULL) {
		std::cout << "cube_matrices was never inited" << std::endl;
	}
	if (cube_shader == NULL) {
		std::cout << "cube_shader was never inited" << std::endl;
	}



	//x_width = 9;// ROW;//rows
	//z_width = 10;//COL;//collums

	if (error) {
		std::cout << "there was a problem initalising vars in the city object" << std::endl;
		while (true);
	}
	else {
		std::cout << "passed all tests" << std::endl;
	}

}

