#include "object_manger.h"

object_manger::object_manger() {
	update_projection = false;
	update_cam = false;
	using_custom_shaders = false;
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);
	common = NULL;
	object_id = 0;

	Time = NULL;
	deltatime = NULL;

	max_cubes = 0;

	draw_cubes = true;
	draw_wall = true;
	draw_wall_with_door = true;
	draw_wall_c = true;
	draw_sidewalk = true;
	draw_light_post = true;
	draw_sideroads = true;
	draw_sky_rail_s = true;
	draw_sky_rail_c = true;
	draw_chicken = true;
	draw_cart = true;
	draw_cannon = true;
	draw_zap_tower = true;
	draw_cursed_chicken = false;//through the normal methode

	cursed = new Shader("cursed.vs", "cursed.fs");
	u_time = 0;
	//demo 1 vars
#ifdef DEMO1
	angle = 0;
	scale = glm::vec3(1, 1, 1);
	shirnk_incr = .0000011;
	move_incr = 3;
	angle_incr = 0.01;
	timmer = 0;

	 float_cubes = false;
	 float_wall = true;
	 float_wall_c = true;
	 float_sidewalk = true;
	 float_light_post = true;
	 float_sideroads = true;
	 float_rail = true;
	 phase_two = false;
	 converge = false;
	 setpoints = false;
	 orbit = false;
	 merge_point = glm::vec3(200,50,100);
#endif // DEMO1

}

object_manger::~object_manger() {
	delete blocked_spots;
	//TODO add the mem clearing 
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
}

void object_manger::draw() {
	common->use();
	common->setMat4("projection", projection);
	common->setMat4("view", view);
	for (int q = 0; q < items.size(); q++) {
		if (items[q]->draw && q != 15) {
			glm::mat4* matrix_temp = items[q]->modelMatrices;
			glBindBuffer(GL_ARRAY_BUFFER, items[q]->buffer);
			if (items[q]->updatemats) {
				glBufferData(GL_ARRAY_BUFFER, items[q]->amount * sizeof(glm::mat4), &matrix_temp[0], GL_STATIC_DRAW);
				items[q]->updatemats = false;
			}

			common->setInt("texture_diffuse1", 0);
			glActiveTexture(GL_TEXTURE0); 
			glBindTexture(GL_TEXTURE_2D, items[q]->model->textures_loaded[0].id);
			for (unsigned int i = 0; i < items[q]->model->meshes.size(); i++)
			{
				glBindVertexArray(items[q]->model->meshes[i].VAO);
				glDrawElementsInstanced(GL_TRIANGLES, items[q]->model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, items[q]->amount);
				glBindVertexArray(0);
			}
		}
	}

}

void object_manger::draw_cursed_ob() {
	cursed->use();
	cursed->setMat4("projection", projection);
	cursed->setMat4("view", view);
	u_time += (*deltatime);
	cursed->setFloat("u_time", u_time);
	int q = 15;
		//if (items[q]->draw) {
			glm::mat4* matrix_temp = items[q]->modelMatrices;
			glBindBuffer(GL_ARRAY_BUFFER, items[q]->buffer);
			if (items[q]->updatemats) {
				glBufferData(GL_ARRAY_BUFFER, items[q]->amount * sizeof(glm::mat4), &matrix_temp[0], GL_STATIC_DRAW);
				items[q]->updatemats = false;
			}

			cursed->setInt("texture_diffuse1", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, items[q]->model->textures_loaded[0].id);
			for (unsigned int i = 0; i < items[q]->model->meshes.size(); i++)
			{
				glBindVertexArray(items[q]->model->meshes[i].VAO);
				glDrawElementsInstanced(GL_TRIANGLES, items[q]->model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, items[q]->amount);
				glBindVertexArray(0);
			}
		//}

}

#ifdef DEMO1
void object_manger::update_demo1() {
	if (draw_cubes) {
		draw_cubes = false;
		items[0]->draw = false;
	}
	float speed = ((*deltatime) * move_incr);
	float angle_speed = ((*deltatime) * move_incr);
	float scale_amount = ((*deltatime) * shirnk_incr);
	//scale.x -= scale_amount;
	//scale.y -= scale_amount;
	//scale.z -= scale_amount;
	if ((!phase_two && !converge)|| orbit) {
		angle += angle_speed;
		if (angle >= 360) {
			angle = 0;
		}
	}
	glm::vec3 moveto;
	glm::vec3 rotate;

	timmer += (*deltatime);

	//std::cout << "time " << timmer << std::endl;
	
	if (!setpoints) {
		srand(glfwGetTime()); // initialize random seed	
		float radius = 150.0;
		float offset = 25.0f;
		int amount = 0;
		int i = 0;

		for (int q = 0; q < items.size(); q++) {
			amount += items[q]->amount;
		}

		for (int q = 0; q < items.size(); q++) {
			for (int w = 0; w < items[q]->amount; w++) {
				float angle = (float)i / (float)amount * 360.0f;
				float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float x = sin(angle) * radius + displacement;
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float z = cos(angle) * radius + displacement;
				i++;
				

				items[q]->item_data[w]->x_m = x;
				items[q]->item_data[w]->y_m = y;
				items[q]->item_data[w]->z_m = z;
			}
		}

		setpoints = true;
	}

	if (float_light_post && (timmer < 4)) {
		floaters.push_back(2);
		float_light_post = false;
		float_rail = false;
		floaters.push_back(6);
		std::cout << "floating light posts" << std::endl;
	}
	if (float_wall && (timmer >= 4 && timmer < 8)) {
		floaters.push_back(3);
		float_wall = false;
		std::cout << "floating wall" << std::endl;
	}
	if (float_wall_c && (timmer >= 8 && timmer < 12)) {
		floaters.push_back(4);
		float_wall_c = false;
		std::cout << "floating wall_c" << std::endl;
	}
	if (float_sidewalk && (timmer >= 12 && timmer < 16)) {
		floaters.push_back(1);
		float_sidewalk = false;
		std::cout << "floating sidewalk" << std::endl;
	}
	if (float_sideroads && (timmer >= 16 && timmer < 20)) {
		floaters.push_back(5);
		float_sideroads = false;
		std::cout << "floating sideraods" << std::endl;
	}
	if (!phase_two && timmer > 25) {
		phase_two = true;
		std::cout << "starting phase two" << std::endl;
		move_incr *= 3;
		timmer = 0;
	}

	if (!orbit && timmer > 25) {
		std::cout << "starting the orbeting" << std::endl;
		orbit = true;
	}
	else {
		//std::cout << "time "<< timmer << std::endl;

	}

	for (int qc = 0; qc < floaters.size(); qc++) {
		int q = floaters[qc];
		if (items[q]->draw) {
			glm::mat4* matrix_temp = items[q]->modelMatrices;
			for (int i = 0; i < items[q]->amount; i++) {
				if (phase_two) {
					if (!converge) {
						int ready = 0;
						merge_point.x = items[q]->item_data[i]->x_m;
						merge_point.y = items[q]->item_data[i]->y_m;
						merge_point.z = items[q]->item_data[i]->z_m;

						if (!(items[q]->item_data[i]->x >= merge_point.x - 1 && items[q]->item_data[i]->x <= merge_point.x - 1)) {

							if (items[q]->item_data[i]->x < merge_point.x) {
								items[q]->item_data[i]->x += speed;
								scale.x = merge_point.x / items[q]->item_data[i]->x;
							}
							else if (items[q]->item_data[i]->x > merge_point.x) {
								items[q]->item_data[i]->x -= speed;
								scale.x = items[q]->item_data[i]->x / merge_point.x;
							}
						}

							if (!(items[q]->item_data[i]->y >= merge_point.y - 1 && items[q]->item_data[i]->y <= merge_point.y - 1)) {

								if (items[q]->item_data[i]->y < merge_point.y) {
									items[q]->item_data[i]->y += speed;
									scale.y = merge_point.y / items[q]->item_data[i]->y;
								}
								else if (items[q]->item_data[i]->y > merge_point.y) {
									items[q]->item_data[i]->y -= speed;
									scale.y = items[q]->item_data[i]->y / merge_point.y;
								}
							}

							if (!(items[q]->item_data[i]->z >= merge_point.z - 1 && items[q]->item_data[i]->z <= merge_point.z - 1)) {

								if (items[q]->item_data[i]->z < merge_point.z) {
									items[q]->item_data[i]->z += speed;
									scale.x = merge_point.z / items[q]->item_data[i]->z;
								}
								else if (items[q]->item_data[i]->z > merge_point.z) {
									items[q]->item_data[i]->z -= speed;
									scale.z = items[q]->item_data[i]->z / merge_point.z;
								}

							}
						
					}
					//x = center_x + radius * cos(angle * (PI / 180));
					//y = center_y + radius * sin(angle * (PI / 180));
					if (converge) {
						if (items[q]->item_data[i]->x > 0) {
							items[q]->item_data[i]->x -= speed;
						}
						else {
							items[q]->item_data[i]->x += speed;
						}
						if (items[q]->item_data[i]->y > 0) {
							items[q]->item_data[i]->y -= speed;
						}
						else {
							items[q]->item_data[i]->y += speed;
						}
						//items[q]->item_data[i]->z -= speed;
					}
					else {
						moveto.x = merge_point.x + items[q]->item_data[i]->x * cos(angle * 3.14 / 180);
						moveto.z = merge_point.z + items[q]->item_data[i]->z * sin(angle * 3.14 / 180);
					}
					moveto.y = items[q]->item_data[i]->y;

					if (scale.x >= 1) {
						scale.x = 1;
					}
					if (scale.y >= 1) {
						scale.y = 1;
					}
					if (scale.z >= 1) {
						scale.z = 1;
					}
					//items[q]->item_data[i]->x = cos(timmer);
					//items[q]->item_data[i]->y = sin(timmer);

					//x(t) = at cos(t), y(t) = at sin(t),

					moveto.x = items[q]->item_data[i]->x;
					moveto.y = items[q]->item_data[i]->y;
					moveto.z = items[q]->item_data[i]->z;
				}
				else {
					std::random_device rd;
					std::mt19937 mt(rd());
					std::uniform_real_distribution<double> distribution(0.0, 3.14);
					double mod = (distribution(mt));
					double rando = (distribution(mt));
					items[q]->item_data[i]->y += mod * speed;// *cos(rando);
					bool rotate_ob = false;

					switch (qc) {
					case 0:
						rotate_ob = !float_wall;
						break;
					case 1:
						rotate_ob = !float_wall_c;
						break;
					case 2:
						rotate_ob = !float_wall;
						break;
					case 3:
						rotate_ob = !float_sideroads;
						break;
					case 4:
						rotate_ob = !float_light_post;
						break;
					}

					if (rotate_ob) {
						mod = (distribution(mt));
						if (mod < .3) {
							items[q]->item_data[i]->x_rot += angle_speed;
						}
						else if (mod < .6) {
							items[q]->item_data[i]->y_rot += angle_speed;
						}
						else {
							items[q]->item_data[i]->z_rot += angle_speed;
						}
						items[q]->item_data[i]->angle += angle_speed;
					}
				}

				//moveto = glm::vec3(1);
				//rotate = glm::vec3(1);

				glm::mat4 temp = glm::mat4(1.0f);
				if (!phase_two) {
					moveto.x = items[q]->item_data[i]->x;
					moveto.y = items[q]->item_data[i]->y;
					moveto.z = items[q]->item_data[i]->z;
				}

				if (orbit) {
					items[q]->item_data[i]->x_rot =1;
				//	items[q]->item_data[i]->y_rot;
				//	items[q]->item_data[i]->z_rot;
					items[q]->item_data[i]->angle += angle_speed;
				}
				rotate.x = items[q]->item_data[i]->x_rot;
				rotate.y = items[q]->item_data[i]->y_rot;
				rotate.z = items[q]->item_data[i]->z_rot;

				if (orbit) {
					temp = glm::rotate(temp, glm::radians(angle2), glm::vec3(0,1,0));
					temp = glm::translate(temp, glm::vec3(moveto.x, moveto.y, moveto.z));
					temp = glm::rotate(temp, glm::radians(items[q]->item_data[i]->angle), rotate);
					angle2 += (angle_speed /100);
					if (angle2 >= 360) {
						angle2 = 0;
					}
				}
				else {
					temp = glm::translate(temp, glm::vec3(moveto.x, moveto.y, moveto.z));
					temp = glm::rotate(temp, glm::radians(items[q]->item_data[i]->angle), rotate);
					//temp = glm::scale(temp, scale);
				}
				matrix_temp[i] = temp;

			}
			items[q]->updatemats = true;
		}
	}
}
#endif // DEMO1

void object_manger::init() {
	std::cout << "creating the object manager" << std::endl;

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the object manager" << std::endl;
		while (true);
	}
	
	if (common == NULL) {
		std::cout << "there was no common shader set" << std::endl;
		while (true);
	}

	blocked_spots = new vector<block_loc*>();

	//create the differnt objects
	create_cube_objects();
	create_sidewalk_objects();
	create_light_post_object();
	create_wall_object();
	create_wall_c_fire();
	create_sidestreet_object();
	create_sky_track_s_object();
	create_sky_track_c_object();
	create_chicken_object();
	create_sky_cart_object();

	create_cannon_object();
	create_zap_tower();

	create_cursed_object_buffer();

	create_wall_door_object();

	std::cout << "finished creating the object manager" << std::endl;
}

void object_manger::create_cursed_object_buffer() {
	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("cursed chicken object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/chicken_cursed/cursed_chicken.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = CURSE_CHICKEN_T;
	temp->draw = draw_cursed_chicken;


	items.push_back(temp);

}

void object_manger::increase_buffer_size() {

}

void object_manger::update_item_matrix(update_pak* data, glm::mat4 given_mat) {

	if (data != NULL && data->item_id < items.size()) {
		//std::cout << "updateing " << data->item_id << ", bufferloc = " << data->buffer_loc << std::endl;
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(data->x_scale, data->y_scale, data->z_scale));
		if (given_mat == glm::mat4(-1.0f)) {
			model = glm::translate(model, glm::vec3(data->x, data->y, data->z));
		}
		else {
			model = given_mat;
		}
		
		items[data->item_id]->modelMatrices[data->buffer_loc] = model;
		items[data->item_id]->updatemats = true;
		//delete data;//clean mem
	}
	else {
		std::cout << "could not update item, item_id out of range" << std::endl;
	}

}

void object_manger::create_cube_objects() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("cube object");
	//creating the log item
	buffer = 0;

	if (max_cubes == 0) {
		buffer_size = 50;
	}
	else {
		buffer_size = max_cubes;
	}

	amount = 0;
	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/cube/cube.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = CUBE_T;
	temp->draw = draw_cubes;

	items.push_back(temp);

}

void object_manger::create_sidewalk_objects() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;

	std::string* item_name_t = new std::string("sidwalk object");

	//creating the alter object
	buffer = 0;
	buffer_size = 200;
	amount = 0;
	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/sidewalk/sidewalk.obj");

	//get the location in respect to the bottom right correner of hte map
	int int_x_loc = 15;
	int int_y_loc = 1;
	int int_z_loc = 15;
	//*2.0 is the cube offset vaule

	
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;

	temp->type = SIDEWALK_T;
	temp->draw = draw_sidewalk;

	items.push_back(temp);

}

void object_manger::create_light_post_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("light post object");
	//creating the log item
	buffer = 0;
	buffer_size = 200;
	amount = 0;
	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/light_post/light post.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	
	temp->type = LIGHT_POST_T;
	temp->draw = draw_light_post;

	items.push_back(temp);

}

void object_manger::create_wall_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("wall object");

	buffer = 0;
	buffer_size = 100;
	amount = 0;
	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/building_parts/wall.obj");

	int int_x_loc = 10;
	int int_y_loc = 2;
	int int_z_loc = 10;

	float x = int_x_loc * 2;
	float y = int_y_loc;
	float z = int_z_loc * 2;

	float x_scale = 1;
	float y_scale = 1;
	float z_scale = 1;
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(x, y, z));
	modelMatrices[0] = trans;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	//temp->item_data.push_back(temp_data);//add the data for the object

	temp->type = WALL_T;
	temp->draw = draw_wall;

	items.push_back(temp);

}

void object_manger::create_wall_c_fire() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("wall_c object");

	buffer = 0;
	buffer_size = 100;
	amount = 0;
	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/building_parts/corner.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;

	temp->type = WALL_C_T;
	temp->draw = draw_wall_c;

	items.push_back(temp);

}

void object_manger::create_sidestreet_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("sidestreet object");

	buffer = 0;
	buffer_size = 100;
	amount = 0;
	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/sidestreet/sidestreet.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = SIDESTREET_T;
	temp->draw = draw_sideroads;

	items.push_back(temp);

}

void object_manger::create_sky_track_s_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("sky rail s object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/sky_tracks/sky_rails_s.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = SKYTRACK_S_T;
	temp->draw = draw_sky_rail_s;


	items.push_back(temp);

}

void object_manger::create_sky_track_c_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("sky rail c object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/sky_tracks/sky_rails_c.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = SKYTRACK_C_T;
	temp->draw = draw_sky_rail_c;


	items.push_back(temp);

}

void object_manger::create_chicken_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("chicken object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/chicken/chicken.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = CHICKEN_T;
	temp->draw = draw_chicken;


	items.push_back(temp);

}

void object_manger::create_sky_cart_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("cart object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/sky_tracks/sky_rails_cart.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = SKYTRACK_CART;
	temp->draw = draw_cart;


	items.push_back(temp);

}

void object_manger::create_cannon_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("cannon_frame object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/machines/cannon_frame.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = CANNON_FRAME_T;
	temp->draw = draw_cannon;

	items.push_back(temp);

	//creating the platform

	item_name_t = new std::string("cannon_lowering platform object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/machines/lower_platform.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = CANNON_PLATFORM_T;
	temp->draw = draw_cannon;

	items.push_back(temp);


	//creating the cannon
	item_name_t = new std::string("cannon object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/machines/cannon.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = CANNON_T;
	temp->draw = draw_cannon;

	items.push_back(temp);

}

void object_manger::create_zap_tower() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("zap_tower object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/machines/zap_tower.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = ZAP_TOWER_T;
	temp->draw = draw_zap_tower;

	items.push_back(temp);

	//creating the zap_sphere

	item_name_t = new std::string("zap_sphere object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/machines/zap_sphere.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = ZAP_SPHERE_T;
	temp->draw = draw_zap_tower;

	items.push_back(temp);

}

void object_manger::create_wall_door_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("wall door object");

	buffer = 0;
	buffer_size = 200;
	amount = 0;

	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/building_parts/wall_with_door.obj");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	item* temp = new item;
	temp->buffer_size = buffer_size;
	temp->buffer = buffer;
	temp->amount = amount;
	temp->model = model;
	temp->modelMatrices = modelMatrices;
	temp->custom_shader = custom_shader;
	temp->item_name = item_name_t;
	temp->type = WALL_D_T;
	temp->draw = draw_wall_with_door;


	items.push_back(temp);

}

std::vector< item_loc>  object_manger::place_items_init() {
	std::cout << "placing the items in the world" << std::endl;
	std::vector< item_loc> output;
	for (int i = 0; i < items.size(); i++) {//each of the differnt items types
		for (int q = 0; q < items[i]->item_data.size(); q++) {
			item_info* unplaced_item = items[i]->item_data[q];
			item_loc temp(unplaced_item, unplaced_item->x_m, unplaced_item->y_m, unplaced_item->z_m);
			output.push_back(temp);
			//objects that take up more than one spot
			switch (unplaced_item->type) {
			case SIDEWALK_T:
				temp = item_loc(unplaced_item, unplaced_item->x_m + 1, unplaced_item->y_m, unplaced_item->z_m);
				output.push_back(temp);
				temp = item_loc(unplaced_item, unplaced_item->x_m - 1, unplaced_item->y_m, unplaced_item->z_m);
				output.push_back(temp);
				temp = item_loc(unplaced_item, unplaced_item->x_m, unplaced_item->y_m, unplaced_item->z_m + 1);
				output.push_back(temp);
				temp = item_loc(unplaced_item, unplaced_item->x_m, unplaced_item->y_m, unplaced_item->z_m - 1);
				output.push_back(temp);
				break;
			}
		}
	}
	std::cout << "finished placing the items in the world" << std::endl;
	return output;
}

void object_manger::delete_item_from_buffer(item_info* it) {
	//std::cout << "buffer size " << items[it->item_id]->buffer_size << std::endl;
	//std::cout << "buffer amount " << items[it->item_id]->amount << std::endl;


	if (it->buffer_loc == items[it->item_id]->amount - 1) {//if it is the last one 
		//std::cout << "last one" << std::endl;
		items[it->item_id]->item_data.pop_back();
		items[it->item_id]->amount--;
	}
	else {
		//std::cout << "not last one" << std::endl;
		unsigned int buffer_loc = it->buffer_loc;
		unsigned int buffer_loc_end = items[it->item_id]->amount - 1;
		std::cout << buffer_loc << std::endl;
		std::cout << buffer_loc_end << std::endl;
		items[it->item_id]->item_data[buffer_loc] = items[it->item_id]->item_data[buffer_loc_end];
		items[it->item_id]->item_data[buffer_loc]->buffer_loc = buffer_loc;
		items[it->item_id]->item_data.pop_back();
		items[it->item_id]->amount--;
		items[it->item_id]->modelMatrices[buffer_loc] = items[it->item_id]->modelMatrices[buffer_loc_end];
	}
	
}

item_info* object_manger::spawn_item(item_type type, int x,int y, int z, glm::mat4 given_mat) {
	unsigned int buffer_loc;
	unsigned int item_id;
	bool stackable = false;
	int max_stack_size;
	//check to see if the buffer is large enough
	float x_f = x * 2;
	float y_f = y * 2;
	float z_f = z * 2;
	switch (type) {
	case CUBE_T:
		if (items[0]->amount >= items[0]->buffer_size) {
			std::cout << "there are too many debug_cubes" << std::endl;
			return NULL;
		}
		item_id = 0;
		buffer_loc = items[0]->amount;
		items[0]->amount++;
		max_stack_size = 3;
		stackable = true;
		break;
	case SIDEWALK_T:
		if (items[1]->amount >= items[1]->buffer_size) {
			std::cout << "there are too many sidewalks" << std::endl;
			return NULL;
		}
		item_id = 1;
		buffer_loc = items[1]->amount;
		items[1]->amount++;
		max_stack_size = 1;
		stackable = false;
		break;
	case LIGHT_POST_T:
		if (items[2]->amount >= items[2]->buffer_size) {
			std::cout << "there are too many lightposts" << std::endl;
			return NULL;
		}
		item_id = 2;
		buffer_loc = items[2]->amount;
		items[2]->amount++;
		max_stack_size = 9;
		stackable = true;
		y_f = 2.7;
		x_f -= 0.1;
		break;
	case WALL_T:
		if (items[3]->amount >= items[3]->buffer_size) {
			std::cout << "there are too many walls" << std::endl;
			return NULL;
		}
		item_id = 3;
		buffer_loc = items[3]->amount;
		items[3]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case WALL_C_T:
		if (items[4]->amount >= items[4]->buffer_size) {
			std::cout << "there are too many wall_c" << std::endl;
			return NULL;
		}
		item_id = 4;
		buffer_loc = items[4]->amount;
		items[4]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case SIDESTREET_T:
		if (items[5]->amount >= items[5]->buffer_size) {
			std::cout << "there are too many sidestreets" << std::endl;
			return NULL;
		}
		item_id = 5;
		buffer_loc = items[5]->amount;
		items[5]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case SKYTRACK_S_T:
		if (items[6]->amount >= items[6]->buffer_size) {
			std::cout << "there are too many skyrail_s" << std::endl;
			return NULL;
		}
		item_id = 6;
		buffer_loc = items[6]->amount;
		items[6]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case SKYTRACK_C_T:
		if (items[7]->amount >= items[7]->buffer_size) {
			std::cout << "there are too many skyrail_C" << std::endl;
			return NULL;
		}
		item_id = 7;
		buffer_loc = items[7]->amount;
		items[7]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case CHICKEN_T:
		if (items[8]->amount >= items[8]->buffer_size) {
			std::cout << "there are too many skyrail_C" << std::endl;
			return NULL;
		}
		item_id = 8;
		buffer_loc = items[8]->amount;
		items[8]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case SKYTRACK_CART:
		if (items[9]->amount >= items[9]->buffer_size) {
			std::cout << "there are too many skyrail_Cart" << std::endl;
			return NULL;
		}
		item_id = 9;
		buffer_loc = items[9]->amount;
		items[9]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case CANNON_FRAME_T:
		if (items[10]->amount >= items[10]->buffer_size) {
			std::cout << "there are too many cannon frames" << std::endl;
			return NULL;
		}
		item_id = 10;
		buffer_loc = items[10]->amount;
		items[10]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case CANNON_PLATFORM_T:
		if (items[11]->amount >= items[11]->buffer_size) {
			std::cout << "there are too many cannon platforms" << std::endl;
			return NULL;
		}
		item_id = 11;
		buffer_loc = items[11]->amount;
		items[11]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case CANNON_T:
		if (items[12]->amount >= items[12]->buffer_size) {
			std::cout << "there are too many cannon " << std::endl;
			return NULL;
		}
		item_id = 12;
		buffer_loc = items[12]->amount;
		items[12]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case ZAP_TOWER_T:
		if (items[13]->amount >= items[13]->buffer_size) {
			std::cout << "there are too many cannon " << std::endl;
			return NULL;
		}
		item_id = 13;
		buffer_loc = items[13]->amount;
		items[13]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case ZAP_SPHERE_T:
		if (items[14]->amount >= items[14]->buffer_size) {
			std::cout << "there are too many cannon " << std::endl;
			return NULL;
		}
		item_id = 14;
		buffer_loc = items[14]->amount;
		items[14]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case CURSE_CHICKEN_T:
		if (items[15]->amount >= items[15]->buffer_size) {
			std::cout << "there are too many cannon " << std::endl;
			return NULL;
		}
		item_id = 15;
		buffer_loc = items[15]->amount;
		items[15]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	case WALL_D_T:
		if (items[16]->amount >= items[16]->buffer_size) {
			std::cout << "there are too many cannon " << std::endl;
			return NULL;
		}
		item_id = 16;
		buffer_loc = items[16]->amount;
		items[16]->amount++;
		max_stack_size = 1;
		stackable = false;
		y_f = 2;
		break;
	}


	item_info* output = new item_info;
	output->amount = 1;
	output->x_m = x;
	output->y_m = 7;
	output->z_m = z;
	output->type = type;
	output->x_scale = 1;
	output->y_scale = 1;
	output->z_scale = 1;
	output->x = x_f;
	output->y = y_f;
	output->z = z_f;
	//output->zone_location = NULL;
	output->item_name = items[item_id]->item_name;
	output->item_id = item_id;
	output->buffer_loc = buffer_loc;
	output->debug_id = object_id;
	output->stackable = stackable;
	output->stack_size = 1;
	output->max_stack_size = max_stack_size;
	object_id++;

	//std::cout << "item at " << x_f << "," << y_f << "," << z_f << std::endl;
	glm::mat4 trans = glm::mat4(-1.0f);
	if (given_mat != trans) {
		trans = given_mat;
	}
	else {
		trans = glm::translate(trans, glm::vec3(x_f, y_f, z_f));
	}
	
	items[item_id]->modelMatrices[buffer_loc] = trans;
	items[item_id]->updatemats = true;
	items[item_id]->item_data.push_back(output);
	return output;
}

void object_manger::merge_item_stacks(item_info* keep, item_info* rm) {
	int total = keep->stack_size + rm->stack_size;
	int diff = keep->max_stack_size - total;
	if (diff >= 0) {
		std::cout << "clean merge" << std::endl;
		//delete_item_from_buffer(rm);
		//delete rm;
		//rm = NULL;
	}
	else {
		std::cout << "not clean merge" << std::endl;
		std::cout << diff*-1<< " left over" << std::endl;
		keep->stack_size = keep->max_stack_size;
		rm->stack_size = diff * -1;
	}
}

void object_manger::split_merge_item_stacks(item_info* keep, item_info* rm) {

}

item_info* object_manger::split_item_stacks(item_info* keep, int amount) {

	return NULL;
}
std::string object_manger::item_type_to_string(item_type i) {
	std::string output = "unkown";

	switch (i)
	{
	case CUBE_T:
		output = "CUBE_T";
		break;
	case SIDEWALK_T:
		output = "SIDEWALK_T";
		break;
	case LIGHT_POST_T:
		output = "LIGHT_POST_T";
		break;
	case WALL_T:
		output = "WALL_T";
		break;
	case WALL_C_T:
		output = "WALL_C_T";
		break;
	case SIDESTREET_T:
		output = "SIDESTREET_T";
		break;
	case SKYTRACK_S_T:
		output = "SKYTRACK_S_T";
		break;
	case SKYTRACK_C_T:
		output = "SKYTRACK_C_T";
		break;
	case CHICKEN_T:
		output = "CHICKEN_T";
		break;
	case SKYTRACK_CART:
		output = "SKYTRACK_CART";
		break;
	case CANNON_FRAME_T:
		output = "CANNON_FRAME_T";
		break;
	case CANNON_PLATFORM_T:
		output = "CANNON_PLATFORM_T";
		break;
	case CANNON_T:
		output = "CANNON_T";
		break;
	case ZAP_TOWER_T:
		output = "ZAP_TOWER_T";
		break;
	case ZAP_SPHERE_T:
		output = "ZAP_SPHERE_T";
		break;
	case CURSE_CHICKEN_T:
		output = "CURSE_CHICKEN_T";
		break;
	}

	return output;
}
