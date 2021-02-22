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
#include <iostream>

timing* Time = NULL;
float* deltaTime = NULL;

int main() {


    Time = new timing(false);

    deltaTime = Time->get_time_change_static();

    bool drawsky = true;
    bool sigle_light_soruce = false;

    std::cout << "creating the objects" << std::endl;

}

