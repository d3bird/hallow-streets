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
	draw_wall_c = true;
	draw_sidewalk = true;
	draw_light_post = true;
	draw_sideroads = true;
}

object_manger::~object_manger() {
	delete blocked_spots;
}

void object_manger::draw() {
	common->use();
	common->setMat4("projection", projection);
	common->setMat4("view", view);
	for (int q = 0; q < items.size(); q++) {
		if (items[q]->draw) {
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

void object_manger::update() {

}

void object_manger::init() {
	std::cout << "creating the object manager" << std::endl;

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the sky" << std::endl;
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
	//create_table_object();
	create_sidestreet_object();

	std::cout << "finished creating the object manager" << std::endl;
}

void object_manger::increase_buffer_size() {

}

void object_manger::update_item_matrix(update_pak* data) {

	if (data != NULL && data->item_id < items.size()) {
		//std::cout << "updateing " << data->item_id << ", bufferloc = " << data->buffer_loc << std::endl;
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(data->x_scale, data->y_scale, data->z_scale));
		model = glm::translate(model, glm::vec3(data->x, data->y, data->z));
		items[data->item_id]->modelMatrices[data->buffer_loc] = model;
		delete data;//clean mem
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
	temp->draw = draw_sideroads;

	items.push_back(temp);

}

void object_manger::create_table_object() {

	unsigned int buffer;
	unsigned int buffer_size;
	unsigned int amount;
	glm::mat4* modelMatrices;
	Shader* custom_shader;
	Model* model;
	std::string* item_name_t = new std::string("table object");

	buffer = 0;
	buffer_size = 10;
	amount = 1;
	modelMatrices = new glm::mat4[buffer_size];
	custom_shader = NULL;
	model = new Model("resources/objects/table/table.obj");

	int int_x_loc = 9;
	int int_y_loc = 2;
	int int_z_loc = 14;

	float x = int_x_loc * 2;
	float y = int_y_loc;
	float z = int_z_loc * 2;

	float x_scale = 1;
	float y_scale = 1;
	float z_scale = 1;
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(x, y, z));
	modelMatrices[0] = trans;

	item_info* temp_data = new item_info;
	temp_data->type = TABLE;
	temp_data->x = x;
	temp_data->y = y;
	temp_data->z = z;
	temp_data->x_m = int_x_loc;
	temp_data->y_m = int_y_loc;
	temp_data->z_m = int_z_loc;
	temp_data->x_scale = x_scale;
	temp_data->y_scale = y_scale;
	temp_data->z_scale = z_scale;
	temp_data->item_id = 0;
	temp_data->buffer_loc = 0;
	temp_data->item_name = item_name_t;
	temp_data->debug_id = object_id;
	//temp_data->zone_location = NULL;
	temp_data->stackable = false;
	temp_data->stack_size = 1;
	temp_data->max_stack_size = 1;
	object_id++;

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
	temp->item_data.push_back(temp_data);//add the data for the object

	//open_tables.push_back(temp_data);

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

item_info* object_manger::spawn_item(item_type type, int x, int z, glm::mat4 given_mat) {
	unsigned int buffer_loc;
	unsigned int item_id;
	bool stackable = false;
	int max_stack_size;
	//check to see if the buffer is large enough
	float x_f = x * 2;
	float y_f = 7;
	float z_f = z * 2;
	switch (type) {
	case CUBE_T:
		if (items[0]->amount >= items[0]->buffer_size) {
			std::cout << "there are too many of these objects" << std::endl;
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
