#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "model.h"
#include "shader.h"
#include "time.h"
#include "networking/network_manager.h"

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};


class text_engine{
public:
	text_engine();
	~text_engine();

	void draw();
	void update();
	void init(network_manager* net = NULL);

	void RenderText(Shader* shader, std::string text, float x, float y, float scale, glm::vec3 color);

	void set_projection(glm::mat4 i) { projection = i; update_projection = true; }
	void set_cam(glm::mat4 i) { view = i; update_cam = true; }

	void set_time(timing* i) { Time = i; }

	void set_typing(bool i) { typing = i; }

	void add_char_to_message(char *i, bool remove = false);

	void send_meeage();
	void recive_message(std::string in);

private:



	glm::mat4 view;
	glm::mat4 projection;
	bool update_projection;
	bool update_cam;

	bool typing;

	std::string message;
	std::string *message_history;
	int max_history;

	network_manager* network;
	timing* Time;
	float* deltatime;

	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	//text rendering data
	std::map<GLchar, Character> Characters;
	unsigned int VAO, VBO;
	FT_Library ft;
	Shader* shader;
};

