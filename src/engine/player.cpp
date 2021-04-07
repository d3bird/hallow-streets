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

