#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "shader.h"
#include "time.h"
#include "object_manger.h"
#include "camera.h"

class stealth{
public:
	stealth();
	~stealth();

	void init(object_manger* o);

	void set_projection(glm::mat4 i) { projection = i; update_projection = true; }
//	void set_cam(glm::mat4 i) { view = i; update_cam = true; }

	void set_time(timing* i) { Time = i; }

	void set_debug_stealth_test(glm::vec3 loc, float angle, int distance);

	bool is_cam_in_veiw(glm::vec3 cam_loc,glm::vec3 loc, float angle, int distance);

	bool is_cam_in_veiw_line_of_sight(glm::vec3 cam_loc, glm::vec3 loc);

	void set_cam(Camera* c) { cam = c; }
	void set_veiw_map(bool** v) { veiw_map = v; }

	float get_cam_loc_x() { return cam->get_pos().x; }
	float get_cam_loc_z() { return cam->get_pos().z; }

private:


	glm::vec2 rotate_point(float cx, float cy, float angle, glm::vec2  p);
	float area(float x1, float y1, float x2, float y2, float x3, float y3);
	bool isInside(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y);

	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;

	timing* Time;
	float* deltatime;

	object_manger *OBJM;
	Camera* cam;

	//debugging
	item_info* cube2;
	item_info* cube3;

	bool** veiw_map;
};

