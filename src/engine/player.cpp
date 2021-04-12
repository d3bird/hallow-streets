#include "player.h"

player::player(Camera* c, bool free){
	update_projection = false;
	update_cam = false;

	Time = NULL;
	deltatime = NULL;

	free_camera = free;
	cam = c;

	height_map = NULL;

	if (free_camera) {
		cam->activate_free_Cam();

	}
	else {
		cam->deactivate_free_Cam();
	}

}

player::~player(){
	delete space;
	delete moon;
}


void player::draw() {
	//space->use();
	//space->setMat4("projection", projection);
	//space->setMat4("view", view);
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(x, y, z));
	////model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	//space->setMat4("model", model);
	//moon->Draw(space);

}

void player::update() {

	if (height_map != NULL) {

	}
}

void player::init() {
	std::cout << "creating player" << std::endl;

	std::cout << "loading shaders" << std::endl;
	space = new Shader("planet.vs", "planet.fs"); 
	std::cout << "loading models" << std::endl;
	moon = new Model("resources/objects/planet/planet.obj");
	glm::vec3* look = cam->get_Front_no_Y();
	if (Time != NULL) {
		std::cout << "setting time" << std::endl;
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the player" << std::endl;
		while (true);
	}

	std::cout << "finished creating player" << std::endl;

}

bool player::did_pos_change() { 
	bool output = false;
	glm::vec3 pos =cam->get_pos();
	int x = ((pos.x / 2) / 8);
	int z = ((pos.z / 2) / 8);
	//std::cout << "player x_cell: "<<x<<" y_cell: "<<z << std::endl;
	if (x_cell != x) {
		x_cell = x;
		output = true;
	}
	if (z_cell != z) {
		z_cell =z;
		output = true;
	}
	return output;
}


int player::get_direction() {

	look_direction i = LOST;

	if ((look->z > -.8 && look->z < -.2) &&
		(look->x > .2 && look->x < .8)) {
		i = NORTH_EAST;
	}
	else if ((look->z > -.8 && look->z < -.2) &&
		(look->x < -.2 && look->x > -.8)) {
		i = NORTH_WEST;
	}
	else if ((look->z < .8 && look->z > .2) &&
		(look->x > .2 && look->x < .8)) {
		i = SOUTH_EAST;
	}
	else if ((look->z < .8 && look->z > .2) &&
		(look->x < -.2 && look->x > -.8)) {
		i = SOUTH_WEST;
	}

	else if (look->z >= .8) {
		i = SOUTH;
	}
	else if (look->z <= -.8) {
		i = NORTH;
	}
	else if (look->x >= .8) {
		i = EAST;
	}
	else if (look->x <= -.8) {
		i = WEST;
	}

	//std::cout << "looking " << look_direction_to_string(i) << ", x_look: " << look->x << " y_look: " << look->z << std::endl;
	return i;
}

std::string player::look_direction_to_string(look_direction i) {
	std::string output = "unkown";
	switch (i)
	{
	case player::NORTH:
		output = "NORTH";
		break;
	case player::NORTH_EAST:
		output = "NORTH_EAST";
		break;
	case player::NORTH_WEST:
		output = "NORTH_WEST";
		break;
	case player::WEST:
		output = "WEST";
		break;
	case player::EAST:
		output = "EAST";
		break;
	case player::SOUTH:
		output = "SOUTH";
		break;
	case player::SOUTH_EAST:
		output = "SOUTH_EAST";
		break;
	case player::SOUTH_WEST:
		output = "SOUTH_WEST";
		break;

	}
	return output;
}
