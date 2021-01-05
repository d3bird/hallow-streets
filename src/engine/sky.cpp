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
	raining = false;
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

	if (raining) {
		rain->use();
		rain->setMat4("projection", projection);
		rain->setMat4("view", view);
		model = glm::mat4(1.0f);
		rain->setMat4("model", model);
		glBindVertexArray(rain_VAO);
		glDrawArrays(GL_POINTS, 0, 4);
	}
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
	space = new Shader("planet.vs", "planet.fs"); 
	moon = new Model("resources/objects/planet/planet.obj");

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the sky" << std::endl;
		while (true);
	}

	std::cout << "finished creating sky/lighting" << std::endl;
	//weather_init();
}

void sky::weather_init() {
	raining = false;
	rain = new Shader("rain.vs", "rain.fs", "rain.gs");
	glLineWidth(3);
	float points[] = {
	-0.5f,  0.5f, 0.1f, 1.0f, 0.0f, 0.0f, // top-left
	 0.5f,  0.5f, 0.1f, 0.0f, 1.0f, 0.0f, // top-right
	 0.5f, -0.5f, 0.1f, 0.0f, 0.0f, 1.0f, // bottom-right
	-0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 0.0f  // bottom-left
	};

	int num_rain_beams = 4;
	int total_spots = num_rain_beams * 6;
	rain_points = new float [total_spots];

	int count = 0;
	float x_loc = 0;
	float y_loc = 7;
	float z_loc = 0;
	for (int i = 0; i < total_spots; i++) {
		switch (count){
		case 0:
			rain_points[i] = x_loc;
			x_loc += 2;
			break;
		case 1:
			rain_points[i] = y_loc;
			break;
		case 2:
			rain_points[i] = z_loc;
			break;
		case 3:
			rain_points[i] = 1;
			break;
		case 4:
		case 5:
			rain_points[i] = 0;
			break;
		}
		count++;
		if (count >= 6) {
			count = 0;
		}
	}

	glGenBuffers(1, &rain_VBO);
	glGenVertexArrays(1, &rain_VAO);
	glBindVertexArray(rain_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, rain_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), rain_points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

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