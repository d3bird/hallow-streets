#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include <string>
#include <vector>
#include <iostream>

class skymap{
public:
	skymap();
	~skymap();
	
	void draw();

	void init();

	void set_projection(glm::mat4 i) { projection = i; update_projection = true; }
	void set_cam(glm::mat4 i) { view = i; }

private:
	
	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int loadTexture(char const* path);

	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;

	Shader* shader;
	Shader* skyboxShader;

	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;

};