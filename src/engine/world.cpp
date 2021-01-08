#include "world.h"


world::world(){

}

world::~world(){
	delete City;
}


void world::draw() {

	//std::cout << "drawling sky" << std::endl;
	Sky->set_cam(view);
	Sky->draw();

	//std::cout << "setting lighting " << std::endl;
	lighting_in->use();
	lighting_in->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	//lighting_in->setVec3("lightPos", Sky->get_light_loc());
	lighting_in->setVec3("lightPos", glm::vec3(10,10,10));
	//lighting_in->setVec3("lightColor", 0.0f, 1.0f, 1.0f);
	lighting_in->setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	//std::cout << "drawling city" << std::endl;
	City->set_cam(view);
	City->draw();

}

void world::update() {
	Sky->update();
}

void world::init() {

	lighting_in = new Shader("lighting_instance.vs", "lighting_instance.fs");

	City = new city();
	City->set_time(Time);
	City->set_projection(projection);
	City->set_cam(view);
	City->set_shader(lighting_in);
	City->init();

	Sky = new sky();
	Sky->set_projection(projection);
	Sky->set_cam(view);
	Sky->set_width(10, 10, 10);
	Sky->set_time(Time);
	Sky->init();

	Sky->pause_time_at_noon();

	std::cout << "finished initing objects" << std::endl;
	std::cout << std::endl;
}

void world::change_projection(glm::mat4 i) {
	projection = i;
	City->set_projection(projection);
	Sky->set_projection(projection);
}