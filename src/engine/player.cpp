#include "player.h"

player::player(Camera* c, bool free){
	update_projection = false;
	update_cam = false;

	Time = NULL;
	deltatime = NULL;

	free_camera = free;
	cam = c;
	player_height = 2;
	height_map = NULL;
	look = NULL;
	dir = -2;
	tolerence = .2;
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
		glm::vec3 pos = cam->get_pos();
		int x = ((pos.x / 2) / 8);
		int z = ((pos.z / 2) / 8);



	}
}

void player::projected_movement(Camera_Movement move, float time) {

	if (height_map != NULL && !free_camera) {
		//std::cout << "checking height" << std::endl;
		bool pass = true;
		glm::vec3 old_pos = cam->get_pos();
		int old_x = ((old_pos.x / 2));
		int old_z = ((old_pos.z / 2));
		double old_height = height_map[old_x][old_z];
		
		glm::vec3 pos = cam->project_movement(move, time);
		int x = ((pos.x / 2));
		int z = ((pos.z / 2));

		double new_height = height_map[x][z];

		cam->set_height(height_map[old_x][old_z]+ player_height);

		//std::cout << "old_x " << old_x << " old_z " << old_z << " | " << " x " << x << " | " << z << std::endl;

		if (new_height > old_height) {
			if (new_height - old_height >= tolerence) {
				pass = false;
			}
		}
		else if(new_height< old_height){
			if (old_height - new_height >= tolerence) {
				pass = false;
			}
		}
		if (pass) {
			cam->ProcessKeyboard(move, time);
			cam->set_height(height_map[x][z] + player_height);
		}
		else {
			//old_pos.x = old_x*2;
			//old_pos.z = old_z*2;
			//cam->set_pos(old_pos);
		}
		//std::cout << "checking height, old: "<< old_height<<" new: "<< new_height << std::endl;
	}
	else {
		cam->ProcessKeyboard(move, time);
	}

}

void player::init() {
	std::cout << "creating player" << std::endl;

	std::cout << "loading shaders" << std::endl;
	space = new Shader("planet.vs", "planet.fs"); 
	std::cout << "loading models" << std::endl;
	moon = new Model("resources/objects/planet/planet.obj");
	look = cam->get_Front_no_Y();
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
	if (look == NULL) {
		look = cam->get_Front_no_Y();
	}
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
