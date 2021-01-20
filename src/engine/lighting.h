#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "model.h"
#include "shader.h"
#include "time.h"

class lighting{
public:
	lighting();
	~lighting();

	void preform_lighting_calcs();

	void update();

	void init();

	//setters
	void set_projection(glm::mat4 i) { projection = i; }
	void set_cam(glm::mat4 i) { view = i; }
	void set_time(timing* i) { Time = i; }

	void set_cam_pos(glm::vec3 i) { cam_pos = i; }

	//getters
	Shader* get_gem_shader() { return shaderGeometryPass; }
	Shader* get_lighting_shader() { return shaderLightingPass; }

private:

	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cam_pos;
	timing* Time;

	bool draw_light_cubes;//the possition of the light sources

	bool update_light_info;

	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;


	//deffered lighting vars
	Shader* shaderGeometryPass;
	Shader* shaderLightingPass;
	Shader* shaderLightBox;

	std::vector<glm::mat4> objectPositions;
	glm::mat4* modelMatrices;
	glm::mat4 model;
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

};

