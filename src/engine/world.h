#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <random>

#include "shader.h"
#include "camera.h"
#include "time.h"
#include "city.h"
#include "sky.h"
#include "text_rendering.h"
#include "object_manger.h"
#include "audio_manger.h"

class world {
public:
	world();
	~world();

	void draw_deferred();
	void draw_single();
	void update();

	void draw_objects();

	void init();

	//misc functions

	void set_single_draw() { single = true; }

	void play_sound_effect(int i) { ADM->play_sound_effect(i); }
	void play_background_music(int i) { ADM->play_background_music(i); }

	void increase_background_music() { ADM->increase_volume(); }
	void decrease_background_music() { ADM->decrease_volume(); }

	//getters

	//setters
	void set_projection(glm::mat4 i) { projection = i; }
	void set_cam(glm::mat4 i) { view = i; }
	void set_time(timing* i) { Time = i; }

	void set_cam_pos(glm::vec3 i) { cam_pos = i; }

	void change_projection(glm::mat4 i);
	void set_camera_obj(Camera* cam) { ADM->set_cam(cam); AM->set_cam(cam); }
	//demo functions

#ifdef DEMO1
	void start_demo_1() { render_text = false; start_demo1 = true; }
#endif // DEMO1



private:

	void lighting_init();

	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cam_pos;
	timing* Time;

	city* City;
	sky* Sky;
	text_engine* text_render;

	audio_manger* ADM;
	object_manger* OBJM;
	animation_manager* AM;
	bool single;
	bool render_text;
	//single source
	Shader* lighting_in;
	glm::vec3 lighting_loc;

	//deferred shading
	bool draw_lights_debug;
	bool update_lights;
	Shader* shaderGeometryPass;
	Shader* shaderLightingPass;
	Shader* shaderLightBox;

	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	std::vector<glm::mat4> objectPositions;
	glm::mat4* modelMatrices;
	Model* backpack;
	unsigned int mod_buffer;
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec;
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	unsigned int rboDepth;
	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

	void renderCube();
	void renderQuad();

	bool draw_speakers;
	std::vector<glm::vec3> speakers_locs;

	//demo data
#ifdef DEMO1
	bool start_demo1;
#endif // DEMO1
};

