#include "stealth.h"

stealth::stealth(){
	update_projection = false;
	update_cam = false;

	Time = NULL;
	deltatime = NULL;

	cube2 = NULL;
	cube3 = NULL;
	OBJM = NULL;
	cam = NULL;
}

stealth::~stealth(){
}


glm::vec2 stealth::rotate_point(float cx, float cy, float angle, glm::vec2  p)
{
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	p.x -= cx;
	p.y -= cy;

	// rotate point
	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + cx;
	p.y = ynew + cy;
	return p;
}

void stealth::set_debug_stealth_test(glm::vec3 loc, float angle, int distance) {
	//std::cout << "setting debug cubes" << std::endl;

	if (cube2 == NULL) {
		std::cout << "cube 2 was null" << std::endl;
		return;
	}
	if (cube3 == NULL) {
		std::cout << "cube 3 was null" << std::endl;
		return;
	}

	update_pak update_pac;

	glm::vec3 current_loc = loc;

	current_loc.x += distance * 2;
	current_loc.z -= (distance-1) * 2;

	//glm::mat4 trans = glm::mat4(1.0f);

	//current_loc *= trans;

	//update cube 2
	update_pac.x = current_loc.x;
	update_pac.y = current_loc.y;
	update_pac.z = current_loc.z;

	update_pac.x_scale = 1;
	update_pac.y_scale = 1;
	update_pac.z_scale = 1;

	update_pac.buffer_loc = cube2->buffer_loc;
	update_pac.item_id = cube2->item_id;

	//glm::vec3 tvec = glm::vec3(-distance, 0.0f, 0.0f);
	//glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
	//float rot_angle = angle / 100.0f;
	////float spin_angle = glm::radians(spin_speed_) / 100.0f;

	//glm::mat4 rotate = glm::mat4(1);
	//rotate = glm::translate(rotate, tvec);
	//rotate = glm::rotate(rotate, angle, axis);
	//rotate = glm::translate(rotate, -tvec);

	glm::vec2 new_test(1.0f);
	new_test.x = current_loc.x;
	new_test.y = current_loc.z;
	new_test = rotate_point(loc.x, loc.z, glm::radians(-angle), new_test);

	glm::mat4 new_mat = glm::translate(glm::mat4(1.0f), glm::vec3(new_test.x, loc.y, new_test.y));

	OBJM->update_item_matrix(&update_pac, new_mat);


	current_loc.x = loc.x;
	current_loc.y = loc.y;
	current_loc.z = loc.z;

	current_loc.x += distance * 2;
	current_loc.z += (distance-1) * 2;

	//trans = glm::mat4(1.0f);
	//current_loc *= trans;

	//update cube 3
	update_pac.x = current_loc.x;
	update_pac.y = current_loc.y;
	update_pac.z = current_loc.z;

	update_pac.x_scale = 1;
	update_pac.y_scale = 1;
	update_pac.z_scale = 1;

	update_pac.buffer_loc = cube3->buffer_loc;
	update_pac.item_id = cube3->item_id;

	new_test = glm::vec2(1.0f);
	new_test.x = current_loc.x;
	new_test.y = current_loc.z;
	new_test = rotate_point(loc.x, loc.z, glm::radians(-angle), new_test);

	new_mat = glm::translate(glm::mat4(1.0f), glm::vec3(new_test.x, loc.y, new_test.y));

	OBJM->update_item_matrix(&update_pac, new_mat);

}

float stealth::area(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

/* A function to check whether point P(x, y) lies inside the triangle formed
   by A(x1, y1), B(x2, y2) and C(x3, y3) */
bool stealth::isInside(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y)
{
	/* Calculate area of triangle ABC */
	float A = area(x1, y1, x2, y2, x3, y3);

	/* Calculate area of triangle PBC */
	float A1 = area(x, y, x2, y2, x3, y3);

	/* Calculate area of triangle PAC */
	float A2 = area(x1, y1, x, y, x3, y3);

	/* Calculate area of triangle PAB */
	float A3 = area(x1, y1, x2, y2, x, y);

	/* Check if sum of A1, A2 and A3 is same as A */
	return (A == A1 + A2 + A3);
}

bool stealth::is_cam_in_veiw(glm::vec3 cam_loc, glm::vec3 loc, float angle, int distance) {
	//std::cout << "checking to see if cam is in veiw" << std::endl;
	if (cam == NULL) {
		//std::cout << "cam is null" << std::endl;
		return false;
	}
	bool output = false;
	glm::vec2 point1 = glm::vec2(1);
	point1.x = loc.x;
	point1.y = loc.z;
	//std::cout << "point1 x: " << point1.x << " z: " << point1.y << std::endl;
	glm::vec2 point2 = glm::vec2(1);

	glm::vec3 current_loc = loc;
	//current_loc.x = loc.x;
	//current_loc.y = loc.y;
	//current_loc.z = loc.z;

	current_loc.x += distance * 2;
	current_loc.z += (distance - 1) * 2;

	point2.x = current_loc.x;
	point2.y = current_loc.z;
	point2 = rotate_point(loc.x, loc.z, glm::radians(-angle), point2);

	//std::cout << "point2 x: " << point2.x << " z: " << point2.y << std::endl;
	glm::vec2 point3 = glm::vec2(1);

	current_loc.x = loc.x;
	current_loc.z = loc.z;

	current_loc.x += distance * 2;
	current_loc.z -= (distance - 1) * 2;

	point3.x = current_loc.x;
	point3.y = current_loc.z;
	point3 = rotate_point(loc.x, loc.z, glm::radians(-angle), point3);

	//std::cout << "point3 x: " << point3.x << " z: " << point3.y << std::endl;


	glm::vec2 player_loc = glm::vec2(cam_loc.x, cam_loc.z);
	////glm::vec3 loc_cam = cam->get_pos();
	//player_loc.x = cam->get_pos().x;
	//player_loc.y = cam->get_pos().z;

	//std::cout << "player_loc x: " << player_loc.x << " z: " << player_loc.y << std::endl;


	output = isInside(point1.x, point1.y,
		point2.x, point2.y,
		point3.x, point3.y,
		player_loc.x, player_loc.y);
	//std::cout << "done" << std::endl;
	return output;
}


void stealth::init(object_manger* o) {
	std::cout << "stealth modaul" << std::endl;

	OBJM = o;

	if (OBJM != NULL) {
		glm::vec3 current_loc(0, 6, -4);

		std::cout << "cube2 was null, creating cube " << std::endl;
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, current_loc);
		cube2 = OBJM->spawn_item(CUBE_T, 0, 6, -4, trans);

		current_loc.x = 6;

		std::cout << "cube3 was null, creating cube " << std::endl;
		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, current_loc);
		cube3 = OBJM->spawn_item(CUBE_T, 6, 6, -4, trans);

	}
	else {
		std::cout << "object manager is null" << std::endl;
	}
	if (Time != NULL) {
		std::cout << "setting time" << std::endl;
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the stealth" << std::endl;
		//while (true);
	}

	std::cout << "finished creating stealth modaul" << std::endl;

}
