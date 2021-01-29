#include "city_gen.h"

city_gen::city_gen(){
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
}

city_gen::~city_gen(){

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

		layout[6][9] = open;//fix a error in the premade street layout
	}
	create_expanded_layout();

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

	srand(time(NULL));

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

	create_buildings(x1,y1, x2, y2);

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

bool city_gen::is_road(int i, int h) {

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


void city_gen::create_expanded_layout() {

	if (key < 1) {
		std::cout << "the key to expand the layout was bellow 1, problems may arise" << std::endl;
	}
	/* key for what the set numbers mean
	 * 1:  place cube
	 * 2:  place wall
	 * 3:  place wall_d
	 * 4:  place wall_c
	 * 5:  place light post
	 * 6:  place road (sidewalk top)
	 * 7:  place road (sidewalk bottom)
	 * 8:  place road (sidewalk left)
	 * 9:  place road (sidewalk right)
	 * 10: place road/path with no sidewalk
	*/

	int set = 0;
	bool exp = true;
	bool walls = false;
	bool corn = false;
	bool road = false;
	bool open_s = false;
	for (int i = 0; i < block_height; i++) {
		for (int h = 0; h < block_width; h++) {
			exp = true;
			walls = false;
			corn = false;
			road = false;
			open_s = false;
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
				if ((layout[i + 1][h] == small_road || layout[i + 1][h] == road_top) && (layout[i][h + 1] == small_road || layout[i][h + 1] == road_top)) {
					corn = true;
				}
				set = 4;
				break;
			case open:
			default:
				set = 0;
				break;
			}
			if (walls) {

				if (corn) {//catch if it was not the propoer corner
					corn = false;
					layout_e[(i * key) + key - 1][(h * key) + key - 1] = set;

				}
				else {
					if (layout[i][h + 1] == small_road || layout[i][h + 1] == road_top) {
						layout_e[i * key][(h * key) + key - 1] = set;
					}
					else if (layout[i + 1][h] == small_road || layout[i + 1][h] == road_top) {
						layout_e[(i * key) + key - 1][h * key] = set;
					}
					else {
						layout_e[i * key][h * key] = set;
					}
				}

			}else if (road) {
				create_road_tile(i, h, set);

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

void city_gen::create_road_tile(int start_x, int start_y, int set) {
	//int set;
	int i = start_x;
	int h = start_y;

	int road_type = set;

	layout_e[i * key][h * key] = set;
	if (key != 1) {
		for (int x = 0; x < key; x++) {
			for (int y = 0; y < key; y++) {
				if (x == 0 && y == 0) {
					layout_e[(i * key) + x][(h * key) + y] = set;
					set = 1;
				}
				else if (layout[i][h] != small_road && x == 1 && y == 2) {//create the street lights
					
					if (gen_last_light) {
						gen_last_light = false;
						layout_e[(i * key) + x][(h * key) + y] = 5;
					}
					else {//no light just a normal spot
						gen_last_light = true;
						layout_e[(i * key) + x][(h * key) + y] = set;
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
