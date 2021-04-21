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

void player::print_heights_around_player() {

	glm::vec3 old_pos = cam->get_pos();
	int old_x = ((old_pos.x / 2));
	int old_z = ((old_pos.z / 2));
	int dist = 4;
	for (int i = old_x - dist; i < old_x + dist; i++) {
		for (int h = old_z - dist; h < old_z + dist; h++) {
			std::cout<<" "<<height_map[i][h];
		}
		std::cout << std::endl;
	}
}

void player::projected_movement(Camera_Movement move, float time) {

	if (false &&height_map != NULL && !free_camera) {
		//std::cout << "checking height" << std::endl;
		bool pass = true;
		int dir = get_direction();

		glm::vec3 old_pos = cam->get_pos();
		int old_x = ((old_pos.x / 2));
		int old_z = ((old_pos.z / 2));
		double height = height_map[old_x][old_z];

		bool down = can_pass_with(height, old_x, old_z + 1);
		bool up = can_pass_with(height, old_x, old_z - 1);
		bool left = can_pass_with(height, old_x - 1, old_z);
		bool right = can_pass_with(height, old_x + 1, old_z);

		glm::vec3 pos = cam->project_movement(move, time);
		int x = ((pos.x / 2));
		int z = ((pos.z / 2));

		if (!down) {
			if (z == old_z + 1) {
				pass = false;
			}
		}
		if (!up) {
			if (z == old_z - 1) {
				pass = false;
			}
		}

		if (!right) {
			if (x == old_x + 1) {
				pass = false;
			}
		}
		if (!left) {
			if (x == old_x - 1) {
				pass = false;
			}
		}

		//switch (move)
		//{
		//case FORWARD:
		//	break;
		//case BACKWARD:
		//	break;
		//case LEFT:
		//	break;
		//case RIGHT:
		//	break;
		//case UP:
		//	break;
		//case DOWN:
		//	break;
		//default:
		//	break;
		//}

		if (pass) {
			cam->ProcessKeyboard(move, time);
		}
		else {
			std::cout << "old_x " << old_x << " old_z " << old_z << std::endl;
		}
	}
	else {
		cam->ProcessKeyboard(move, time);
	}

}

void player::projected_movement4(Camera_Movement move, float time) {

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
				std::cout << "old_x " << old_x << " old_z " << old_z << " | " << " x " << x << " | " << z << std::endl;
			}
		}
		else if(new_height< old_height){
			if (old_height - new_height >= tolerence) {
				pass = false;
				std::cout << "old_x " << old_x << " old_z " << old_z << " | " << " x " << x << " | " << z << std::endl;
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

bool player::can_pass_with(double heigh,int i, int h) {
	double new_height = height_map[i][h];
	bool pass = true;
	if (new_height > heigh) {
		if (new_height - heigh >= tolerence) {
			pass = false;
		}
	}
	else if (new_height < heigh) {
		if (heigh - new_height >= tolerence) {
			pass = false;
		}
	}
	return pass;
}

int player::determ_dirction_moving(glm::vec3& pos, glm::vec3& new_pos) {
	double heigh;
	double new_height;
	bool pass;
	int output = 0;
	double diff = 0;

	pass = false;
	heigh = pos.x;
	new_height = new_pos.x;
	if (new_height > heigh) {
		if (new_height - heigh >= tolerence) {
			pass = true;
			diff = new_height - heigh;
		}
	}
	else if (new_height < heigh) {
		if (heigh - new_height >= tolerence) {
			pass = true;
			diff = heigh - new_height;
		}
	}
	if (pass) {
		if (pos.x > new_pos.x) {
			output = 1;//west
		}
		else {
			output = 2;//east
		}
	}

	pass = false;
	heigh = pos.z;
	new_height = new_pos.z;
	if (new_height > heigh) {
		if (new_height - heigh >= tolerence) {
			pass = true;
			diff = new_height - heigh;
		}
	}
	else if (new_height < heigh) {
		if (heigh - new_height >= tolerence) {
			pass = true;
			diff = heigh - new_height;
		}
	}
	if (pass) {
		if (pos.z > new_pos.z) {
			output = 3;//down
		}
		else {
			output = 4;//up
		}
	}

	return output;
}

void player::projected_movement2(Camera_Movement move, float time) {

	if (height_map != NULL && !free_camera) {
		//std::cout << "checking height" << std::endl;
		bool pass = true;
		glm::vec3 old_pos = cam->get_pos();
		int old_x = ((old_pos.x / 2));
		int old_z = ((old_pos.z / 2));
		double old_height = height_map[old_x][old_z];

		glm::vec3 pos = cam->project_movement(move, time);
		//int x = ((pos.x / 2));
		//int z = ((pos.z / 2));

		int x = ((old_pos.x / 2));
		int z = ((old_pos.z / 2));
		bool up  = can_pass_with(old_height, x, z-1);
		bool down = can_pass_with(old_height, x, z+1);
		bool east = can_pass_with(old_height, x-1, z);
		bool west = can_pass_with(old_height, x+1, z);

		double new_height = height_map[x][z];

		//int dir = determ_dirction_moving(old_pos, pos);
		//std::cout << "going dir: "<< dir << std::endl;
		if (!up) {
			if (pos.z <= (z - 1) * 2) {
				pass = false;
			}
		}
		if (!down) {
			if (pos.z >= (z + 1) * 2) {
				pass = false;
			}
		}
		if (!west) {
			if (pos.x >= ((x - 1) * 2)-2) {
				pass = false;
			}
			else {
				std::cout << "pos.x "<< pos.x<<" | (x - 1) * 2 "<< (x - 1) * 2 << std::endl;
			}
		}
		if (!east) {
			if (pos.x <= ((x + 1) * 2)+2) {
				pass = false;
			}
			else {
				std::cout << "pos.x " << pos.x << " | (x - 1) * 2 " << (x - 1) * 2 << std::endl;
			}
		}
		//cam->set_height(height_map[old_x][old_z] + player_height);

		//std::cout << "old_x " << old_x << " old_z " << old_z << " | " << " x " << x << " | " << z << std::endl;

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

bool player::change_cube(glm::vec3& pos, int dir) {
	bool output = false;
	float offset = 1;
	int old_x = ((pos.x / 2));
	int old_z = ((pos.z / 2));

	int new_x = old_x;
	int new_z = old_z;

	switch (dir) {
	case 0://up
		new_x = (((pos.x) / 2));
		new_z = ((pos.z - offset / 2));
		break;
	case 1://west
		new_x = (((pos.x - offset) / 2));
		new_z = (((pos.z) / 2));
		break;
	case 2://east
		new_x = (((pos.x + offset) / 2));
		new_z = (((pos.z) / 2));
		break;
	case 3://down
		new_x = (((pos.x) / 2));
		new_z = (((pos.z + offset) / 2));
		break;
	}

	if ((new_x != old_x) || (new_z != old_z)) {
		output = true;

	}
	return output;
}

void player::projected_movement3(Camera_Movement move, float time) {

	if (height_map != NULL && !free_camera) {
		//std::cout << "checking height" << std::endl;
		bool pass = true;
		glm::vec3 old_pos = cam->get_pos();
		int old_x = ((old_pos.x / 2));
		int old_z = ((old_pos.z / 2));
		if (old_x < 0 || old_z <= 0) {
			std::cout << " out of bounds" << std::endl;
			cam->ProcessKeyboard(move, time);
			cam->set_height(4+ player_height);
			return;
		}
		double old_height = height_map[old_x][old_z];

		glm::vec3 pos = cam->project_movement(move, time);
		int x = ((pos.x / 2));
		int z = ((pos.z / 2));

		double new_height;

		cam->set_height(height_map[old_x][old_z] + player_height);

		//std::cout << "old_x " << old_x << " old_z " << old_z << " | " << " x " << x << " | " << z << std::endl;

		//dose the player enter a new cube
		bool up = change_cube(old_pos, 0);
		if (up) {
			double new_height = height_map[x][z-1];

			if (new_height > old_height) {
				if (new_height - old_height >= tolerence) {
					pass = false;

				}
			}
		}

		bool west = change_cube(old_pos, 1);
		if (west){
			double new_height = height_map[x-1][z];

			if (new_height > old_height) {
				if (new_height - old_height >= tolerence) {
					pass = false;

				}
			}
		}
		bool east = change_cube(old_pos, 2);
		if (east) {
			double new_height = height_map[x+1][z];

			if (new_height > old_height) {
				if (new_height - old_height >= tolerence) {
					pass = false;

				}
			}
		}
		bool down = change_cube(old_pos, 3);
		if (down) {
			double new_height = height_map[x][z + 1];

			if (new_height > old_height) {
				if (new_height - old_height >= tolerence) {
					pass = false;

				}
			}
		}


		if (down && up && east && west) {
			std::cout << "the player is stuck" << std::endl;
		}

		/*if (new_height > old_height) {
			if (new_height - old_height >= tolerence) {
				pass = false;
			}
		}
		else if (new_height < old_height) {
			if (old_height - new_height >= tolerence) {
				pass = false;
			}
		}*/

		if (pass) {
			cam->ProcessKeyboard(move, time);
			cam->set_height(height_map[x][z] + player_height);
		}
		else {
			old_pos.x = old_x*2;
			old_pos.z = old_z*2;
			//cam->set_height(height_map[x][z] + player_height);
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
