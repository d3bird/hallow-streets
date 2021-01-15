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
			layout[i][h] = big_road;
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
		create_city_block(1, 1, block_width - 1, block_height - 1);

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

	int width = x2 - x1;
	int height = y2 - y1;

	bool creating = true;
	int direct = 0;

	//create the roads from the top
	int top_start = width / 2;
	int parts = top_start;
	int z = top_start;
	int x = 0;

	std::cout << width << " " << top_start << std::endl;
	//create the top road
	for (int i = 1; i < parts; i++) {
		z = top_start * i;
		if (i % 2 == 0) {
			create_road(x, z, direct, width / 2, 3);
		}
		else {
			create_road(x, z, direct, width / 2, 2);
		}
	}

	//ceate the bottom roads
	direct = 1;
	x = width+1;
	top_start = width / 3;
	z = top_start;
	parts = top_start;
	for (int i = 1; i < parts; i++) {
		z = top_start * i;
		if (i % 2 == 0) {
			create_road(x, z, direct, width / 2, 3);
		}
		else {
			create_road(x, z, direct, width / 2, 2);
		}
	}

	create_buildings();

}

void city_gen::create_road(int x, int z, int direct, int mid, int dir_change) {
	bool creating = true;
	bool hit_mid = false;

	while (creating) {

		switch (direct) {
		case 0://down
			if (!hit_mid && x == mid) {
				direct = dir_change;
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
			}
			else {
				x++;
			}
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

		if (layout[x][z] == big_road || layout[x][z] == small_road) {
			creating = false;
		}
		else {
			layout[x][z] = small_road;
		}
	}
}

void city_gen::create_buildings() {
	std::cout << "creating buildings" << std::endl;
	//layout[1][5] = wall;
	//layout[1][1] = wall_c;
	//layout[2][1] = wall;

	for (int i = 0; i < 6; i++) {
		for (int h = 0; h < 6; h++) {
			if (layout[i][h] == open) {
				//check for cornners 
				if ((layout[i - 1][h] == small_road || layout[i - 1][h] == big_road) && (layout[i][h - 1] == small_road || layout[i][h - 1] == big_road)) {
					layout[i][h] = wall_c;
				}else if ((layout[i + 1][h] == small_road || layout[i + 1][h] == big_road) && (layout[i][h - 1] == small_road || layout[i][h - 1] == big_road)) {
					layout[i][h] = wall_c;
				}
				else if ((layout[i + 1][h] == small_road || layout[i + 1][h] == big_road) && (layout[i][h + 1] == small_road || layout[i][h + 1] == big_road)) {
					layout[i][h] = wall_c;
				}
				else if ((layout[i - 1][h] == small_road || layout[i - 1][h] == big_road) && (layout[i][h + 1] == small_road || layout[i][h + 1] == big_road)) {
					layout[i][h] = wall_c;
				}
				else {

					//check for single roads
					if (layout[i - 1][h] == small_road || layout[i - 1][h] == big_road) {
						layout[i][h] = wall;
					}
					else if (layout[i + 1][h] == small_road || layout[i + 1][h] == big_road) {
						layout[i][h] = wall;
					}
					else if (layout[i][h - 1] == small_road || layout[i][h - 1] == big_road) {
						layout[i][h] = wall;
					}
					else if (layout[i][h + 1] == small_road || layout[i][h + 1] == big_road) {
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
		std::cout << "the key to expand the layout was bellow 1" << std::endl;
	}

	int set = 0;
	bool exp = true;
	bool walls = false;
	bool corn = false;
	for (int i = 0; i < block_height; i++) {
		for (int h = 0; h < block_width; h++) {
			exp = true;
			walls = false;
			corn = false;
			switch (layout[i][h]) {
			case big_road:
			case small_road:
				set = 1;
				break;
			case wall:
			case wall_d:
				walls = true;
				set = 3;
				break;
			case wall_c:
				walls = true;
				if ((layout[i + 1][h] == small_road || layout[i + 1][h] == big_road) && (layout[i][h + 1] == small_road || layout[i][h + 1] == big_road)) {
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

				/*if (corn) {
					if ((layout[i - 1][h] == small_road || layout[i - 1][h] == big_road) && (layout[i][h - 1] == small_road || layout[i][h - 1] == big_road)) {
						std::cout << "corn 1" << std::endl;
					}
					else if ((layout[i + 1][h] == small_road || layout[i + 1][h] == big_road) && (layout[i][h - 1] == small_road || layout[i][h - 1] == big_road)) {
						std::cout << "corn 2" << std::endl;

					}
					else if ((layout[i + 1][h] == small_road || layout[i + 1][h] == big_road) && (layout[i][h + 1] == small_road || layout[i][h + 1] == big_road)) {
						std::cout << "corn 3" << std::endl;

					}
					else if ((layout[i - 1][h] == small_road || layout[i - 1][h] == big_road) && (layout[i][h + 1] == small_road || layout[i][h + 1] == big_road)) {
						std::cout << "corn 4" << std::endl;

					}
					corn = false;
				}*/

				if (corn) {//catch if it was not the propoer corner
					corn = false;
					layout_e[(i * key) + key - 1][(h * key) + key - 1] = set;

				}
				else {
					if (layout[i][h + 1] == small_road || layout[i][h + 1] == big_road) {
						layout_e[i * key][(h * key) + key - 1] = set;
					}
					else if (layout[i + 1][h] == small_road || layout[i + 1][h] == big_road) {
						layout_e[(i * key) + key - 1][h * key] = set;
					}
					else {
						layout_e[i * key][h * key] = set;
					}
				}

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

void city_gen::print_layout() {
	std::cout << "printing the city layout" << std::endl;

	for (int i = 0; i < block_height; i++) {
		for (int h = 0; h < block_width; h++) {
			switch (layout[i][h]) {
			case big_road:
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
