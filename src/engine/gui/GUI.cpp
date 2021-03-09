#include "GUI.h"

GUI::GUI() {
	Time = NULL;
	deltatime = NULL;
	gui_window = NULL;

	online = false;
	server = false;
}

GUI::~GUI()
{
}


void GUI::draw() {
  /*  gui_window->use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 1);*/
}

void GUI::update() {


}

void GUI::init() {
	
	//gui_window = new Shader("GUI.vs", "GUI.fs", "GUI.gs");

 //   float points[] = {
 //      -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
 //       0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
 //       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
 //      -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
 //   };
 //   glGenBuffers(1, &VBO);
 //   glGenVertexArrays(1, &VAO);
 //   glBindVertexArray(VAO);
 //   glBindBuffer(GL_ARRAY_BUFFER, VBO);
 //   glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
 //   glEnableVertexAttribArray(0);
 //   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
 //   glEnableVertexAttribArray(1);
 //   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
 //   glBindVertexArray(0);

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the GUI" << std::endl;
	}
}

