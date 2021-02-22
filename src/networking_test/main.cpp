#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include "time.h"
#include "network_manager.h"

#include <string>
#include <iostream>
#include <iterator>
#include <algorithm>

timing* Time = NULL;
float* deltaTime = NULL;

int main() {


    Time = new timing(false);

    deltaTime = Time->get_time_change_static();


    network_manager* network = new network_manager();

    network->init();

    network->open_connetion();

    while (true);

}

