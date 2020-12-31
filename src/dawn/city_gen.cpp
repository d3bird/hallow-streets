#include "city_gen.h"

city_gen::city_gen(){
	update_projection = false;
	update_cam = false;
	Time = NULL;
	deltatime = NULL;

	block_width = 22;
	block_height = 14;

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

	create_city_block(1, 1, block_width-1, block_height-1);

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