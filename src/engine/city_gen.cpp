#include "city_gen.h"

city_gen::city_gen() {
	update_projection = false;
	update_cam = false;
	Time = NULL;
	deltatime = NULL;

	block_width = 22;
	block_height = 14;

	premade = false;
	key = 8;//do note that if this number is changed, the modles will also have to be changed  
	gen_last_light = true;
	max_cubes = block_width * block_height * key * key;

	//rail information

	rail_amount = 25;
	start_r_x = -1;
	start_r_y = -1;
	chicken_pen_amount = 0;

	//cell data 
	generate_cell_info = true;
	layout_cells = NULL;
	//generated_building = NULL;
	height_map = NULL;
	pass_key = NULL;
	obj_count = 43;
}

city_gen::~city_gen() {
	for (int i = 0; i < rails.size(); i++) {
		delete rails[i];
	}
	/*if (generated_building != NULL) {
		for (int i = 0; i < generated_building->cell_info.size(); i++) {
			for (int q = 0; q < generated_building->cell_info[i]->items_on_ground.size(); q++) {
				delete generated_building->cell_info[i]->items_on_ground[q];
			}
			for (int q = 0; q < generated_building->cell_info[i]->items_on_wall.size(); q++) {
				delete generated_building->cell_info[i]->items_on_wall[q];
			}
		}
	}*/
}


void city_gen::draw() {

}

void city_gen::update() {

		//angle += (angle_incr * (*deltatime));
		//std::cout << "angle of the moon "<< angle << std::endl;
}

void city_gen::init() {
	std::cout << "creating the city" << std::endl;

	layout = new city_square*[block_height];
	for (int i = 0; i < block_height; i++) {
		layout[i] = new city_square[block_width];
	}

	if (generate_cell_info) {
		layout_cells = new cell_data * [block_height];
		for (int i = 0; i < block_height; i++) {
			layout_cells[i] = new cell_data[block_width];
		}
	}

	for (int i = 0; i < block_height; i++) {
		for (int h = 0; h < block_width; h++) {
			if ((i == 0 && h == 0) || (i == block_height - 1 && h == 0) || (i == 0 && h == block_width - 1) || (i == block_height - 1 && h == block_width - 1)) {
				layout[i][h] = road_curve;
			}
			else if ((i == 1 && h == 1) || (i == block_height - 2 && h == 1) || (i == 1 && h == block_width - 2) || (i == block_height - 2 && h == block_width - 2)) {
				layout[i][h] = road_inside_curve;
			}
			else if ((h == 0 && i == block_height - 2))  {//special case
				layout[i][h] = road_left;
			}
			else if ((h == block_width-2 && i == block_height - 1)) {//special case
				layout[i][h] = road_bot;
			}
			else if ((h == block_width - 1 && i == block_height - 2)) {//special case
				layout[i][h] = road_right;
			}
			else if ((h == block_width - 1 && i == 1)) {//special case
				layout[i][h] = road_right;
			}
			else if (i == 0 || i == block_height - 2) {
				layout[i][h] = road_top;
			}
			else if (h == 0 || h == block_width - 2) {
				layout[i][h] = road_left;
			}
			else if (i == 1 || i == block_height - 1) {
				layout[i][h] = road_bot;

			} else if (h == 1 || h == block_width - 1) {
				layout[i][h] = road_right;
			}
			else {
				layout[i][h] = open;
			}

		}
	}
	//create the expanded version of the layout
	layout_e = new int * [block_height* key];
	for (int i = 0; i < block_height* key; i++) {
		layout_e[i] = new int[block_width* key];
	}

	for (int i = 0; i < block_height * key; i++) {
		for (int h = 0; h < block_width * key; h++) {
			layout_e[i][h] = 0;
		}
	}

	if (premade) {
		use_premade_map();
	}
	else {
		create_city_block(2, 2, block_width - 2, block_height - 2);

		//layout[6][9] = open;//fix a error in the premade street layout
	}
	create_expanded_layout();

	rail_section* rail_s;
	if (start_r_x != -1 && start_r_y != -1) {
		start_r_x *= 2;
		start_r_y *= 2;
		for (int i = 0; i < rail_amount; i++) {
			rail_s = new rail_section;
			rail_s->loc = glm::vec3(start_r_x + (0 * 2), 4, start_r_y - (i * 2));
			rail_s->rot = glm::vec3(0.0f, 1.0f, 0.0f);
			rail_s->angle = 90;
			if (i == 49) {
				rail_s->type = 1;
			}
			rails.push_back(rail_s);
		}
	}
	else {
		std::cout << "program never found where ro place the rails" << std::endl;
	}

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the city" << std::endl;
		while (true);
	}
	std::cout << "finished the city" << std::endl;
}

void city_gen::create_city_block(int x1, int y1, int x2, int y2) {
	int temp = x1;
	x1 = y1;
	y1 = temp;

	temp = x2;
	x2 = y2;
	y2 = temp;


	//wipe the block
	for (int i = x1; i < x2; i++) {
		for (int h = y1; h < y2; h++) {
			layout[i][h] = open;
		}
	}

	int width = x2 - x1;//up down 
	int height = y2 - y1;//left right

	std::cout << "creating a city block" << std::endl;
	std::cout << "width: " << width << std::endl;
	std::cout << "height: "<< height << std::endl;

	bool creating = true;
	int direct = 0;//inital direction
	int direct_change = 2;
	int amount_roads = 3;

	//how far away from the top of the map that 
	int offset_x = 2;
	int offset_y = 2;


	//start possitions
	int pos_x = height / amount_roads;
	int pos_y = 1;

	//roads starting from the top
	create_road(pos_y, pos_x, direct, (width / 2)+ offset_x, direct_change);
	direct_change = 3;
	create_road(pos_y, height- pos_x , direct, (width /2)+ offset_x, direct_change);

	//roads starting from the bottom
	direct = 1;
	pos_y = width +offset_y;

	create_road(pos_y, pos_x+1 , direct, (width /2)+ offset_x, direct_change);//cut through the middle
	direct_change = 2;
	create_road(pos_y, height - pos_x, direct, (width / 2) + offset_x+1, direct_change);

	create_chicken_pen(7,7, 13,13);

	//creates one building in the spot layed out for it
	//generated_building = generate_building(2,2,7,6);
	//marks and reserves spaces for buildings 
	find_space_for_buildings(x1, y1, x2, y2);

	for (int i = 0; i < build_data.size(); i++){
		buildings.push_back(generate_building(build_data[i]));
		delete build_data[i];
	}
	build_data.clear();

	//creates the walls and corners where the roads are (old
	//create_buildings(x1,y1, x2, y2);

}

void city_gen::find_space_for_buildings(int x1, int y1, int x2, int y2) {
	std::cout << "finding space for buildings" << std::endl;
	int number_of_building_spots = 0;
	bool marking = false;
	int size = 0;
	int x_start = -1;
	int y_start = -1;
	int x_end = -1;
	int y_end = -1;

	int i_t;
	int h_t;

	building_build_data* temp_data;
	//reserve the spot for the buildings
	for (int i = x1; i < x2; i++) {
		for (int h = y1; h < y2; h++) {
			if (layout[i][h] == open) {
				marking = true;
				number_of_building_spots++;
				x_start = i;
				y_start = h;
				i_t = i;
				h_t = h;
				size = 0;
				while (marking) {
					if (layout[i_t][h_t] != open) {
						marking = false;
					}
					else {
						layout[i_t][h_t] = reserverd;
						size++;
						i_t++;
						if (i_t >= x2 || layout[i_t][h_t] != open) {
							//x_end = i_t - 1;
							x_end = i_t;
							i_t = i;
							h_t++;
							if (h_t >= y2 || layout[i_t][h_t] != open) {
								//y_end = h_t - 1;
								y_end = h_t;
								marking = false;
							}
						}
					}
				}
				std::cout << "found space for building x_s: " << x_start << " y_s: " << y_start
					<< " x_e " << x_end << " y_e " << y_end << " ( " << size << " cells" << std::endl;
				
				temp_data = new building_build_data;
				temp_data->x_start = x_start;
				temp_data->y_start = y_start;
				temp_data->x_end = x_end;
				temp_data->y_end = y_end;
				temp_data->size = size;
				build_data.push_back(temp_data);
			}
		}
	}

	std::cout << "total potental buildings "<< number_of_building_spots << std::endl;
	std::cout << "building data created " << build_data.size() << std::endl;
	std::cout << "finihed finding space for buildings" << std::endl;
}

void city_gen::create_road(int x, int z, int direct, int mid, int dir_change, bool debug) {
	bool creating = true;
	bool hit_mid = false;

	while (creating) {
		if (debug && !hit_mid) {
			std::cout << "x: " << x << " mid: " << mid << std::endl;
		}

		if (!hit_mid && x == mid) {
			direct = dir_change;
		}

		switch (direct) {
		case 0://down
				x++;
			break;
		case 1://up
			x--;
			break;
		case 2://left
			z--;
			break;
		case 3://right
			z++;
			break;
		}

		if (is_road(x,z)) {
			creating = false;
		}
		else {
			layout[x][z] = small_road;
		}
	}
}

bool city_gen::is_road(int i, int h,int road_type) {
	if (road_type == -1) {//all roads
		switch (layout[i][h]) {
		case road_top:
		case road_bot:
		case road_left:
		case road_right:
		case road_curve:
		case road_inside_curve:
		case small_road:
			return true;
			break;
		}
	}
	else if (road_type == 0) {//main roads
		switch (layout[i][h]) {
		case road_top:
		case road_bot:
		case road_left:
		case road_right:
		case road_curve:
		case road_inside_curve:
			return true;
			break;
		}
	}
	else if (road_type == 1) {//side roads
		switch (layout[i][h]) {
		case small_road:
			return true;
			break;
		}
	}
	return false;
}

void city_gen::create_buildings(int i_start, int h_start,int i_max, int h_max) {
	std::cout << "creating buildings" << std::endl;

	for (int i = i_start; i < i_max; i++) {
		for (int h = h_start; h < h_max; h++) {
			if (layout[i][h] == open) {
				//check for cornners 
				if ((is_road(i - 1, h) || is_road(i - 1, h)) && (is_road(i, h - 1) || is_road(i, h - 1))) {
					layout[i][h] = wall_c;
				}
				else if ((is_road(i + 1, h) || is_road(i + 1, h)) && (is_road(i, h - 1) || is_road(i, h - 1))) {
					layout[i][h] = wall_c;
				}
				else if ((is_road(i + 1, h) || is_road(i + 1, h)) && (is_road(i, h + 1) || is_road(i, h + 1))) {
					layout[i][h] = wall_c;
				}
				else if ((is_road(i - 1, h) || is_road(i - 1, h)) && (is_road(i, h + 1) || is_road(i, h + 1))) {
					layout[i][h] = wall_c;
				}
				else {

					//check for single roads
					if (is_road(i - 1, h)) {
						layout[i][h] = wall;
					}
					else if (is_road(i + 1, h)) {
						layout[i][h] = wall;
					}
					else if (is_road(i, h - 1)) {
						layout[i][h] = wall;
					}
					else if (is_road(i, h + 1)) {
						layout[i][h] = wall;
					}
				}
			}
		}
	}
	//while (true);
}

void city_gen::create_chicken_pen(int i_start, int h_start, int i_max, int h_max) {
	std::cout << "creating chicken pen" << std::endl;

	for (int i = i_start; i < i_max; i++) {
		for (int h = h_start; h < h_max; h++) {
			if (layout[i][h] == open) {
				layout[i][h] = chicken_pen;
			}
		}
	}
	std::cout << "finished creating chicken pen" << std::endl;

	//while (true);
}

void city_gen::create_expanded_layout() {

	if (key < 1) {
		std::cout << "the key to expand the layout was bellow 1, problems may arise" << std::endl;
	}

	/* key for what the set numbers mean
	 * 1:  place cube
	 * 2:  place wall
	 * 3:  place wall_d
	 * 4:  place wall_c no rotate
	 * 5:  place light post
	 * 6:  place road (sidewalk top)
	 * 7:  place road (sidewalk bottom)
	 * 8:  place road (sidewalk left)
	 * 9:  place road (sidewalk right)
	 * 10: place road/path with no sidewalk
	 * 11: place light post 90 degree trun
	 * 12: place light post 180 degree trun
	 * 13: place light post 270 degree trun
	 * 14: place wall_c 90 degree trun
	 * 15: place wall_c 180 degree trun
	 * 16: place wall_c 270 degree trun
	 * 17: chicken
	 * 18 chicken ground
	*/

	int set = 0;
	bool exp = true;
	bool walls = false;
	bool corn_bot_right = false;
	bool road = false;
	bool open_s = false;
	bool chickens = false;
	bool grab_info_from_cells = false;
	rail_section* rail_s;
	for (int i = 0; i < block_height; i++) {
		for (int h = 0; h < block_width; h++) {
			exp = true;
			walls = false;
			corn_bot_right = false;
			road = false;
			open_s = false;
			chickens = false;
			grab_info_from_cells = false;
			switch (layout[i][h]) {
			case road_top:
				set = 6;
				road = true;
				break;
			case small_road://should be 10 but it is not modeled yet
				set = 10;
				road = true;
				break;
			case road_bot:
				set = 7;
				road = true;
				break;
			case road_left:
				set = 8;
				road = true;
				break;
			case road_right:
				set = 9;
				road = true;
				break;
			case wall:
			case wall_d:
				walls = true;
				set = 3;
				break;
			case wall_c:
				walls = true;
				/*if ((layout[i + 1][h] == small_road || layout[i + 1][h] == road_top) && (layout[i][h + 1] == small_road || layout[i][h + 1] == road_top)) {
					corn_bot_right = true;
				}
				set = 4;
				*/
				if (is_road(i - 1, h) && is_road(i, h - 1)) {
					set = 15;
				}else if (is_road(i - 1, h) && is_road(i, h + 1)) {
					set = 14;
				}else if (is_road(i + 1, h) && is_road(i, h - 1)) {
					set = 16;
				}
				else {
					set = 4;
					corn_bot_right = true;
				}
				break;
			case chicken_pen:
				chickens = true;
				break;
			case reserverd://cell information
				grab_info_from_cells = true;
				break;
			case open:
			default:
				set = 0;
				break;
			}
			if (walls) {

				if (corn_bot_right) {//catch if it was not the propoer corner
					corn_bot_right = false;
					layout_e[(i * key) + key - 1][(h * key) + key - 1] = set;

				}
				else {
					if (is_road(i,h + 1)) {
						layout_e[i * key][(h * key) + key - 1] = set;
					}
					else if (is_road(i+1, h)) {
						layout_e[(i * key) + key - 1][h * key] = set;
					}
					else {
						layout_e[i * key][h * key] = set;
					}
				}

			}else if (road) {
				create_road_tile(i, h, set);
			}
			else if (chickens) {
				int connecting_pens = 0;
				//check for connection
				if (layout[i - 1][h] == chicken_pen) {
					connecting_pens++;
				}
				 if (layout[i][h + 1] == chicken_pen) {
					connecting_pens++;
				}
				 if (layout[i][h - 1] == chicken_pen) {
					connecting_pens++;
				}
				 if (layout[i + 1][h] == chicken_pen) {
					connecting_pens++;
				}
				//std::cout << "there are " << connecting_pens << " connecting_pens" << std::endl;
				if (connecting_pens == 2) {
					if (layout[i][h+1] == chicken_pen && layout[i+1][h] == chicken_pen) {
						create_chicken_tile(i, h, 1);
					}
					else if (layout[i][h - 1] == chicken_pen && layout[i + 1][h] == chicken_pen) {
						create_chicken_tile(i, h, 3);
					}
					else if (layout[i][h + 1] == chicken_pen && layout[i - 1][h] == chicken_pen) {
						create_chicken_tile(i, h, 2);
					}
					else {
						create_chicken_tile(i, h, 4);
					}
				}
				else {
					create_chicken_tile(i, h, 0);
				}
				
			}
			else if (grab_info_from_cells) {
				create_tile_from_cell(i, h);
			}
			else {
				layout_e[i * key][h * key] = set;
				if (key != 1) {
					for (int x = 0; x < key; x++) {
						for (int y = 0; y < key; y++) {
							layout_e[(i * key) + x][(h * key) + y] = set;
						}
					}
				}
			}
		}
	}

}

void city_gen::create_tile_from_cell(int i, int h) {
	//std::cout << "creating tile from cell" << std::endl;
	cell_data cell = layout_cells[i][h];
	if (cell.expanded_layout_info != NULL) {
		int set = cell.expanded_layout_info[0][0];
		layout_e[i * key][h * key] = set;
		if (key != 1) {
			for (int x = 0; x < key; x++) {
				for (int y = 0; y < key; y++) {
					set = cell.expanded_layout_info[x][y];
					layout_e[(i * key) + x][(h * key) + y] = set;
				}
			}
		}
	}
	else {
		//std::cout << "cell was null" << std::endl;
	}

}

void city_gen::create_chicken_tile(int start_x, int start_y, int type) {
	//int set;
	int i = start_x;
	int h = start_y;

	int set = 1;
	int num_chicks = 4;

	int space = key / 3;

	bool top_blank = false;
	bool bot_blank = false;
	bool rig_blank = false;
	bool lef_blank = false;
	//determin which side to leave blank

	chicken_pen_amount++;
	if (chicken_pen_amount == 2) {
		start_r_x = i * key;
		start_r_y = h * key;
		start_r_x += 1;
		start_r_y += 2;
	}

	if (type == 1) {
		top_blank = true;
		rig_blank = true;
	}
	else if (type == 2) {
		rig_blank = true;
		bot_blank = true;
	}
	else if (type == 3) {
		top_blank = true;
		lef_blank = true;
	}
	else if (type == 4) {
		lef_blank = true;
		bot_blank = true;
	}
	
	if (key != 1) {
		for (int x = 0; x < key; x++) {
			for (int y = 0; y < key; y++) {
				//if ((y != 0 && y != key - 1) && (x != 0 && x != key - 1)) {
					layout_e[(i * key) + x][(h * key) + y] = 18;
				//}
				if (top_blank && x == 0) {
					layout_e[(i * key) + x][(h * key) + y] = 0;
				}
				if (bot_blank && x == key-1) {
					layout_e[(i * key) + x][(h * key) + y] = 0;
				}
				if (rig_blank && y == 0) {
					layout_e[(i * key) + x][(h * key) + y] = 0;
				}
				if (lef_blank && y == key - 1) {
					layout_e[(i * key) + x][(h * key) + y] = 0;
				}

				
			}
		}

		//place the corrners
		if (type == 1) {
			layout_e[(i * key)][(h * key)] = 15;
		}
		else if (type == 2) {
			layout_e[(i * key) + 7][(h * key)] = 16;
		}
		else if (type == 3) {
			layout_e[(i * key)][(h * key) + 7] = 14;
		}
		else if (type == 4) {
			layout_e[(i * key) + 7][(h * key) + 7] = 4;
		}

		//place the chickens
		layout_e[(i * key) + 3][(h * key) + 3] = 17;
		layout_e[(i * key) + 3][(h * key) + 6] = 17;
		layout_e[(i * key) + 6][(h * key) + 3] = 17;
		layout_e[(i * key) + 6][(h * key) + 6] = 17;

	}

}

void city_gen::create_road_tile(int start_x, int start_y, int set) {
	//int set;
	int i = start_x;
	int h = start_y;

	int road_type = set;
	bool placed_light = false;
	bool place_light = false;

	int light_type = 5;

	switch (road_type) {
	case 6://top (90 deg)
		light_type = 13;
		break;
	case 7://bottom
		light_type = 11;
		break;
	case 8://left (no turn)
		light_type = 5;
		break;
	case 9://right
		light_type = 12;
		break;
	}

	//create the inital spaces for the path finding AI
	//note this is a temp solution until the bug next code is fixed
	//it is not placing cubes until the light post is placed
	for (int x = 0; x < key; x++) {
		for (int y = 0; y < key; y++) {
			layout_e[(i * key) + x][(h * key) + y] = 1;
		}
	}

	gen_last_light = (i % 2 == 1 && h % 2 == 0) || (i % 2 == 0 && h % 2 == 1);

	layout_e[i * key][h * key] = set;
	if (key != 1) {
		for (int x = 0; x < key; x++) {
			for (int y = 0; y < key; y++) {
				if (x == 0 && y == 0) {
					layout_e[(i * key) + x][(h * key) + y] = set;
					set = 1;
				}
				else if (!placed_light && layout[i][h] != small_road) {//create the street lights

					switch (road_type) {
					case 6://top 
						if (x == 2 && y == 1) {
							place_light = true;
						}
						break;
					case 7://bottom
						if (x == 5 && y == 1) {
							place_light = true;
						}
						break;
					case 8://left 
						if (x == 1 && y == 2) {
							place_light = true;
						}
						break;
					case 9://right
						if (x == 1 && y == 5) {
							place_light = true;
						}
						break;
					}

					//x == 1 && y == 2
					if (place_light) {
						if (gen_last_light) {
							gen_last_light = false;
							layout_e[(i * key) + x][(h * key) + y] = light_type;
						}
						else {//no light just a normal spot
							gen_last_light = true;
							layout_e[(i * key) + x][(h * key) + y] = set;
						}
						placed_light = true;
						place_light = false;
					}
				}
				else {
					layout_e[(i * key) + x][(h * key) + y] = set;
				}
			}
		}
	}

}

void city_gen::create_wall_tile(int start_x, int start_y) {

	
}

void city_gen::create_empty_tile(int start_x, int start_y) {


}


void city_gen::print_layout() {
	std::cout << "printing the city layout" << std::endl;

	for (int i = 0; i < block_height; i++) {
		for (int h = 0; h < block_width; h++) {
			switch (layout[i][h]) {
			case road_top:
			case road_bot:
			case road_left:
			case road_right:
			case road_curve:
			case road_inside_curve:
				std::cout << "R ";
				break;
			case small_road:
				std::cout << "r ";
				break;
			case wall:
				std::cout << "w ";
				break;
			case wall_c:
				std::cout << "c ";
				break;
			case wall_d:
				std::cout << "d ";
				break;
			case chicken_pen:
				std::cout << "C ";
				break;
			case open:
				std::cout << ". ";
				break;
			default:
				std::cout << "- ";
				break;
			}
		}
		std::cout << std::endl;
	}

}

void city_gen::print_expanded_layout() {
	std::cout << "printing the expanded city layout" << std::endl;

	for (int i = 0; i < block_height*key; i++) {
		for (int h = 0; h < block_width * key; h++) {
			std::cout << layout_e[i][h] << " ";
		}
		std::cout << std::endl;
	}

}

void city_gen::use_premade_map() {

}

bool city_gen::check_buiding_data(building_build_data* buiding_data) {
	bool output = true;
	if (buiding_data != NULL) {
		if (buiding_data->x_start == -1 ||//bad cords
			buiding_data->y_start == -1 ||
			buiding_data->x_end == -1 ||
			buiding_data->y_end == -1) {
			output = false;
		}
		else if (buiding_data->size == -1) {//bad generation data
			output = false;
		}
	}
	else {
		output = false;
	}

	return output;
}


building* city_gen::generate_building(building_build_data* buiding_data) {
	int start_x = buiding_data->x_start;
	int start_y = buiding_data->y_start;
	int end_x = buiding_data->x_end;
	int end_y = buiding_data->y_end;

	if (!check_buiding_data(buiding_data)) {
		std::cout << "build data was incomplete or invlad" << std::endl;
		return NULL;
	}

	building* output = NULL;

		std::cout << "generating building" << std::endl;
		output = new building;
		output->build_type = workshop;
		output->dis_type = industry;

		//the different generation types vars
		bool loading_dock = true;
		bool dock_main_road = true;
		bool dock_side_road = true;
		int dock_side = -1;
		bool walls_along_edges = true;
		//what are the boundries of the buildings
		int wall_x_start = start_x;
		int wall_y_start = start_y;
		int wall_x_end = end_x;
		int wall_y_end = end_y;
		bool spawn_door = true;
		int door_side = -1;
		bool door_spawned = false;
		int door_spots_left = -1;
		//roof type
		bool has_roof = true;
		int roof_type;//0 = wavy 1 = traditional slant, 2 pointing roof, 3 = flat  
		int roof_subtype =0;//changes the direction the roof is facing
		roof_type = 2;
		int number_floors =2;

		//set the differnt vars for differnt type of generations 
		if (loading_dock) {
			std::vector<int> sides_with_road;
			if (dock_main_road) {
				if (is_road(start_x - 1, start_y, 0)) {
					std::cout << "main road north" << std::endl;
					sides_with_road.push_back(0);
				}
				if (is_road(start_x, start_y - 1, 0)) {
					std::cout << "main road west" << std::endl;
					sides_with_road.push_back(1);
				}
				if (is_road(end_x + 1, end_y, 0)) {
					std::cout << "main road south" << std::endl;
					sides_with_road.push_back(3);
				}
				if (is_road(end_x, end_y + 1, 0)) {
					std::cout << "main road east" << std::endl;
					sides_with_road.push_back(4);
				}
			}
			if (dock_side_road) {
				if (is_road(start_x - 1, start_y, 1)) {
					std::cout << "side road north" << std::endl;
					sides_with_road.push_back(0);
				}
				if (is_road(start_x, start_y - 1, 1)) {
					std::cout << "side road west" << std::endl;
					sides_with_road.push_back(1);
				}
				if (is_road(end_x + 1, end_y, 1)) {
					std::cout << "side road south" << std::endl;
					sides_with_road.push_back(3);
				}
				if (is_road(end_x, end_y + 1, 1)) {
					std::cout << "side road east" << std::endl;
					sides_with_road.push_back(4);
				}
			}
			if (sides_with_road.size() > 0) {
				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_real_distribution<double> distribution(0, sides_with_road.size());
				dock_side = (int)distribution(mt);
				dock_side = sides_with_road[dock_side];
				dock_side = 1;
				switch (dock_side) {
				case 0://north side of the building
					wall_x_start += 1;
					break;
				case 1://west side of the building
					wall_y_start += 1;
					break;
				case 3://south side of te building
					wall_x_end -= 1;
					break;
				default://east side of the building
					wall_y_end -= 1;
					dock_side = 4;
					break;
				}
			}
			else {
				std::cout << "can not create a loading dock, no main road" << std::endl;
			}
		}

		if (spawn_door) {
			std::vector<int> sides_with_road;
			if (is_road(start_x - 1, start_y, 0)) {
				std::cout << "main road north" << std::endl;
				sides_with_road.push_back(0);
			}
			if (is_road(start_x, start_y - 1, 0)) {
				std::cout << "main road west" << std::endl;
				sides_with_road.push_back(1);
			}
			if (is_road(end_x + 1, end_y, 0)) {
				std::cout << "main road south" << std::endl;
				sides_with_road.push_back(3);
			}
			if (is_road(end_x, end_y + 1, 0)) {
				std::cout << "main road east" << std::endl;
				sides_with_road.push_back(4);
			}


			if (sides_with_road.size() > 0) {
				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_real_distribution<double> distribution(0, sides_with_road.size());
				door_side = (int)distribution(mt);
				door_side = sides_with_road[door_side];
				switch (dock_side) {
				case 0://north/south side of the building
				case 3:
					door_spots_left = (wall_x_end - 2) - (wall_x_start + 1);
					std::cout << door_spots_left << " spots along north/south for door" << std::endl;
					break;
				case 1://west/east side of the building
				default:
					door_spots_left = (wall_y_end - 2) - (wall_y_start + 1);
					std::cout << door_spots_left << " spots along west/east for door" << std::endl;
					break;
				}
			}
			else {
				std::cout << "can not create a door site, no road" << std::endl;
			}
		}

		int x_number_between_walls = wall_x_end - wall_x_start;
		int x_remaining_roofs = x_number_between_walls;

		int y_number_between_walls = wall_y_end - wall_y_start;
		int y_remaining_roofs = y_number_between_walls;
		int int_temp =0;

		for (int i = start_x; i < end_x; i++) {
			for (int h = start_y; h < end_y; h++) {
				layout[i][h] = reserverd;

				cell_data cell;
				cell.x_loc = i;
				cell.y_loc = h;

				cell.expanded_layout_info = new int* [key];
				for (int i = 0; i < key; i++) {
					cell.expanded_layout_info[i] = new int[key];
				}

				//setting the basaeline for the square
				for (int q = 0; q < key; q++) {
					for (int x = 0; x < key; x++) {
						cell.expanded_layout_info[q][x] = -1;
					}
				}

				//fill in the squares
				for (int q = 0; q < key; q++) {
					for (int x = 0; x < key; x++) {
						if (cell.expanded_layout_info[q][x] == -1) {
							//place walls
							if (walls_along_edges) {
								if ((i == wall_x_start || i == wall_x_end - 1) ||
									(h == wall_y_start || h == wall_y_end - 1)) {

									if (i == wall_x_start && h == wall_y_start) {
										cell.type = wall_c;
										if (q == 0 && x == 0) {
											cell.expanded_layout_info[q][x] = 15;
											if (number_floors > 1) {
												item_gen_info* temp_data = new item_gen_info;
												temp_data->item = wall;
												temp_data->floor = 2;
												temp_data->angle = 180;
												temp_data->expanded_layout_value = 15;
												cell.items_on_wall.push_back(temp_data);
											}
										}
										else {
											if (q == 4 && x == 0) {
												cell.expanded_layout_info[q][x] = 38;
											}
											else
												if (q == 0 || x == 0) {
													cell.expanded_layout_info[q][x] = 0;
												}
												else {
													cell.expanded_layout_info[q][x] = 1;
												}
										}
									}
									else if (i == wall_x_start && h == wall_y_end - 1) {
										cell.type = wall_c;
										if (q == 0 && x == key - 1) {
											cell.expanded_layout_info[q][x] = 14;
											if (number_floors > 1) {
												item_gen_info* temp_data = new item_gen_info;
												temp_data->item = wall;
												temp_data->floor = 2;
												temp_data->angle = 90;
												temp_data->expanded_layout_value = 15;
												cell.items_on_wall.push_back(temp_data);
											}
										}
										else {
											if (q == 0 || x == key - 1) {
												cell.expanded_layout_info[q][x] = 0;
											}
											else {
												cell.expanded_layout_info[q][x] = 1;
											}
										}
									}
									else if (i == wall_x_end - 1 && h == wall_y_start) {
										cell.type = wall_c;
										if (q == key - 1 && x == 0) {
											cell.expanded_layout_info[q][x] = 16;
											if (number_floors > 1) {
												item_gen_info* temp_data = new item_gen_info;
												temp_data->item = wall;
												temp_data->floor = 2;
												temp_data->angle = 180;
												temp_data->expanded_layout_value = 16;
												cell.items_on_wall.push_back(temp_data);
											}
										}
										else {
											if (q == key - 1 || x == 0) {
												cell.expanded_layout_info[q][x] = 0;
											}
											else {
												cell.expanded_layout_info[q][x] = 1;
											}
										}
									}
									else if (i == wall_x_end - 1 && h == wall_y_end - 1) {
										cell.type = wall_c;
										if (q == key - 1 && x == key - 1) {
											cell.expanded_layout_info[q][x] = 4;
											if (number_floors > 1) {
												item_gen_info* temp_data = new item_gen_info;
												temp_data->item = wall;
												temp_data->floor = 2;
												temp_data->angle = 0;
												temp_data->expanded_layout_value = 4;
												cell.items_on_wall.push_back(temp_data);
											}
										}
										else {
											if (q == key - 1 || x == key - 1) {
												cell.expanded_layout_info[q][x] = 0;
											}
											else {
												cell.expanded_layout_info[q][x] = 1;
											}
										}
									}
									else {
										//makesure that the walls spawn in the wall area and not outside
										if (i >= wall_x_start && i < wall_x_end && h >= wall_y_start && h < wall_y_end) {
											cell.type = wall;

											if (i == wall_x_start) {
												if (q == 0 && x == 0) {
													if (spawn_door && !door_spawned) {
														cell.expanded_layout_info[q][x] = 27;
														door_spawned = true;
														if (number_floors > 1) {
															item_gen_info* temp_data = new item_gen_info;
															temp_data->item = wall;
															temp_data->floor = 2;
															temp_data->angle = 270;
															temp_data->expanded_layout_value = 22;
															cell.items_on_wall.push_back(temp_data);
														}
													}
													else {
														if (dock_side == 0) {
															cell.expanded_layout_info[q][x] = 31;
															cell.type = wall_loading;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 270;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
														else {
															cell.expanded_layout_info[q][x] = 22;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 270;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
													}
												}
												else {
													if (q == 0) {
														cell.expanded_layout_info[q][x] = 0;
													}
													else {
														cell.expanded_layout_info[q][x] = 1;
													}
												}
											}
											else if (i == wall_x_end - 1) {
												if (q == key - 1 && x == 0) {
													if (spawn_door && !door_spawned) {
														cell.expanded_layout_info[q][x] = 27;
														door_spawned = true;
														if (number_floors > 1) {
															item_gen_info* temp_data = new item_gen_info;
															temp_data->item = wall;
															temp_data->floor = 2;
															temp_data->angle = 270;
															temp_data->expanded_layout_value = 22;
															cell.items_on_wall.push_back(temp_data);
														}
													}
													else {
														if (dock_side == 3) {
															cell.expanded_layout_info[q][x] = 31;
															cell.type = wall_loading;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 270;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
														else {
															cell.expanded_layout_info[q][x] = 22;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 270;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
													}
												}
												else {
													if (q == key - 1) {
														cell.expanded_layout_info[q][x] = 0;
													}
													else {
														cell.expanded_layout_info[q][x] = 1;
													}
												}
											}
											else if (h == wall_y_start) {
												if (q == 0 && x == 0) {
													if (spawn_door && !door_spawned) {
														cell.expanded_layout_info[q][x] = 26;
														door_spawned = true;
														if (number_floors > 1) {
															item_gen_info* temp_data = new item_gen_info;
															temp_data->item = wall;
															temp_data->floor = 2;
															temp_data->angle = 180;
															temp_data->expanded_layout_value = 22;
															cell.items_on_wall.push_back(temp_data);
														}
													}
													else {
														if (dock_side == 1) {
															cell.expanded_layout_info[q][x] = 30;
															cell.type = wall_loading;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 180;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
														else {
															cell.expanded_layout_info[q][x] = 21;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 180;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
													}
												}
												else {
													if (dock_side == 1) {
														if (x == 0 && (q == 4 || q == 6)) {
															cell.expanded_layout_info[q][x] = 40;//the loading doors
														}
													}
													else
														if (x == 0) {
															cell.expanded_layout_info[q][x] = 0;
														}
														else {
															cell.expanded_layout_info[q][x] = 1;
														}
												}
											}
											else if (h == wall_y_end - 1) {
												if (q == 0 && x == key - 1) {
													if (spawn_door && !door_spawned) {
														cell.expanded_layout_info[q][x] = 26;
														door_spawned = true;
														if (number_floors > 1) {
															item_gen_info* temp_data = new item_gen_info;
															temp_data->item = wall;
															temp_data->floor = 2;
															temp_data->angle = 180;
															temp_data->expanded_layout_value = 22;
															cell.items_on_wall.push_back(temp_data);
														}
													}
													else {
														if (dock_side == 4) {
															cell.expanded_layout_info[q][x] = 30;
															cell.type = wall_loading;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 180;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
														else {
															cell.expanded_layout_info[q][x] = 21;
															if (number_floors > 1) {
																item_gen_info* temp_data = new item_gen_info;
																temp_data->item = wall;
																temp_data->floor = 2;
																temp_data->angle = 180;
																temp_data->expanded_layout_value = 22;
																cell.items_on_wall.push_back(temp_data);
															}
														}
													}
												}
												else {
													if (x == key - 1) {
														cell.expanded_layout_info[q][x] = 0;
													}
													else {
														cell.expanded_layout_info[q][x] = 1;
													}
												}
											}
										}
										else {
											cell.expanded_layout_info[q][x] = 1;
										}
									}
								}
								else {
									//test to see if it is inside the walls or outside
									if ((i >= wall_x_start && i < wall_x_end) &&
										(h >= wall_y_start && h < wall_y_end)) {
										cell.expanded_layout_info[q][x] = 18;
									}
									else {//outside the walls
										if (loading_dock) {

											if (dock_side == 0) {

												if (q == key - 2 && x == 0) {
													cell.expanded_layout_info[q][x] = 35;
												}
												else {
													cell.expanded_layout_info[q][x] = 1;
												}
											}
											else if (dock_side == 1) {
												if (q == key - 1 && x == key - 2) {
													cell.expanded_layout_info[q][x] = 32;
												}
												else {
													cell.expanded_layout_info[q][x] = 1;
												}
											}
											else if (dock_side == 3) {
												if (q == key - 1 && x == key - 2) {
													cell.expanded_layout_info[q][x] = 32;
												}
												else {
													cell.expanded_layout_info[q][x] = 1;
												}
											}
											else {
												if (q == key - 1 && x == key - 2) {
													cell.expanded_layout_info[q][x] = 32;
												}
												else {
													cell.expanded_layout_info[q][x] = 1;
												}
											}
										}
										else {
											cell.expanded_layout_info[q][x] = 1;
										}

									}

								}
							}

						}
					}
				}

				//create the roof
				if (has_roof) {

					//test to see if it is inside the walls or outside
					if ((i >= wall_x_start && i < wall_x_end) &&
						(h >= wall_y_start && h < wall_y_end)) {

						item_gen_info* temp_data = new item_gen_info;
						temp_data->item = roof_place;
						temp_data->roof = true;
						bool add_filler = false;
						switch (roof_type)
						{
						case 1://traditional
							if (h == wall_y_end - 1) {
								temp_data->angle = 90;
								temp_data->roof_value = 1;
								temp_data->z_cube_offset = 7;
							}
							else if (h == wall_y_start) {
								temp_data->angle = 270;
								temp_data->roof_value = 1;
								temp_data->x_cube_offset = 7;
							}
							else {
								temp_data->angle = 0;
								temp_data->roof_value = 3;
								temp_data->y_cube_offset = 1;
								temp_data->z_cube_offset = 7;
								if (i == wall_x_start || i == wall_x_end - 1) {
									add_filler = true;
								}
							}
							break;
						case 2://traditional pointy
							if (h == wall_y_end - 1) {
								temp_data->angle = 90;
								temp_data->roof_value = 1;
								temp_data->z_cube_offset = 7;
								
							}
							else if (h == wall_y_start) {
								temp_data->angle = 270;
								temp_data->roof_value = 1;
								temp_data->x_cube_offset = 7;
							}
							else {
								if (y_number_between_walls % 2 == 0) {//if even
									int dis = h - wall_y_start;
									int dis2 = wall_y_end - 1 - h;
									if (dis > dis2) {
										temp_data->angle = 90;
										temp_data->z_cube_offset = 7;
										temp_data->y_cube_offset = dis2+1;
										int_temp = dis2 + 1;
									}
									else {
										temp_data->angle = 270;
										temp_data->x_cube_offset = 7;
										temp_data->y_cube_offset = dis+1;
										int_temp = dis + 1;

									}
									if (i == wall_x_start || i == wall_x_end - 1) {
										add_filler = true;
									}
									temp_data->roof_value = 1;
								}
								else {
									temp_data->angle = 0;
									temp_data->roof_value = 3;
									temp_data->y_cube_offset = 1;
									temp_data->z_cube_offset = 7;
									if (i == wall_x_start || i == wall_x_end - 1) {
										add_filler = true;
									}
								}
							}
							break;
						case 3:
							temp_data->angle = 0;
							temp_data->roof_value = 3;
							temp_data->z_cube_offset = 7;
							break;
						default://jagged
							temp_data->angle = 0;
							temp_data->roof_value = 1;
							break;
						}
						temp_data->floor = number_floors;
						cell.items_on_wall.push_back(temp_data);
						if (add_filler) {
							if (int_temp == 0) {
								temp_data = new item_gen_info;
								temp_data->item = roof_place;
								temp_data->roof = true;
								temp_data->floor = number_floors;
								//temp_data->y_cube_offset = 1;
								temp_data->roof_value = 2;
								temp_data->angle = 90;
								if (i == wall_x_end - 1) {
									temp_data->z_cube_offset = 7;
								}
								cell.items_on_wall.push_back(temp_data);
							}
							else {
								for (int a = 0; a < int_temp; a++) {
									temp_data = new item_gen_info;
									temp_data->item = roof_place;
									temp_data->roof = true;
									temp_data->y_cube_offset = a;
									temp_data->roof_value = 2;
									temp_data->floor = number_floors;
									temp_data->angle = 90;
									if (i == wall_x_end - 1) {
										temp_data->z_cube_offset = 7;
									}
									cell.items_on_wall.push_back(temp_data);
								}
								int_temp = 0;
							}
						}

					}
					else {//outside the walls
						
					}
				}
				layout_cells[i][h] = cell;
				output->cell_info.push_back(&cell);
			}
		}

		std::cout << "finishing generating building" << std::endl;

	return output;
}


double** city_gen::get_height_map() {

	return height_map;
}


bool* city_gen::get_pas_key() {
	if (pass_key == NULL) {
		pass_key = new bool[43];//based off of the the expanded layout
		//nothing
		pass_key[0] = false;
		//cube
		pass_key[1] = true;
		//walls
		pass_key[2] = false;
		pass_key[3] = false;
		pass_key[4] = false;
		//light post
		pass_key[5] = false;
		//roads
		pass_key[6] = true;
		pass_key[7] = true;
		pass_key[8] = true;
		pass_key[9] = true;
		pass_key[10] = true;
		//light post
		pass_key[11] = false;
		pass_key[12] = false;
		pass_key[13] = false;
		//corner
		pass_key[14] = false;		
		pass_key[15] = false;
		pass_key[16] = false;
		//chicken
		pass_key[17] = false;
		//chicken ground
		pass_key[18] = false;
		//more walls
		pass_key[19] = false;
		pass_key[20] = false;
		pass_key[21] = false;
		pass_key[22] = false;
		pass_key[23] = false;
		pass_key[24] = false;
		pass_key[25] = false;
		pass_key[26] = false;
		pass_key[27] = false;
		pass_key[28] = false;
		pass_key[29] = false;
		pass_key[30] = false;
		//floor
		pass_key[31] = true;
		pass_key[32] = true;
		pass_key[33] = true;
		pass_key[34] = true;
		//wall clock
		pass_key[35] = true;
		pass_key[36] = true;
		pass_key[37] = true;
		pass_key[38] = true;
		//loading doors
		pass_key[39] = true;
		pass_key[40] = true;
		pass_key[41] = true;
		pass_key[42] = true;

	}

	return pass_key;
}
