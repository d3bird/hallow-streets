#include "sky.h"

sky::sky(){
	update_projection = false;
	update_cam = false;
	angle = 0;
	Time = NULL;
	deltatime = NULL;
	angle_incr = 2;
	toggled_day = true;
	toggled_night = false;
	paused = false;

}

sky::~sky(){
	delete space;
	delete moon;
}


void sky::draw() {
	space->use();
	space->setMat4("projection", projection);
	space->setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x, y, z));
	//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	space->setMat4("model", model);
	moon->Draw(space);

}

void sky::update() {

	if (!paused) {
		x = center_x + radius * cos(angle * (PI / 180));
		y = center_y + radius * sin(angle * (PI / 180));

		if (!toggled_day && angle >= 0 && angle < 180) {
			Time->set_day();
			toggled_day = true;
			toggled_night = false;
		}
		else if (!toggled_night && angle >= 180 && angle <= 360) {
			Time->set_night();
			toggled_day = false;
			toggled_night = true;
		}

		angle += (angle_incr * (*deltatime));
		//std::cout << "angle of the moon "<< angle << std::endl;
		if (angle >= 360) {
			angle = 0;
		}
	}
}

void sky::init() {
	std::cout << "creating sky/lighting" << std::endl;
	center_x = x_width;
	center_z = z_width;
	x = center_x;
	z = center_z;
	y = center_y;
	radius = x_width + (x_width * 0.5);
	std::cout << "loading shaders" << std::endl;
	space = new Shader("planet.vs", "planet.fs"); 
	std::cout << "loading models" << std::endl;
	moon = new Model("resources/objects/planet/planet.obj");

	if (Time != NULL) {
		std::cout << "setting time" << std::endl;
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the sky" << std::endl;
		while (true);
	}

	std::cout << "finished creating sky/lighting" << std::endl;

}

void sky::pause_time_at_noon() {
	angle = 90;
	paused = true;
	Time->set_day();
	toggled_day = true;
	toggled_night = false;
	x = center_x + radius * cos(angle * (PI / 180));
	y = center_y + radius * sin(angle * (PI / 180));
}