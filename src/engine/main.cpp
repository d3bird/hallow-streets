#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "world.h"
#include "time.h"
#include "skymap.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_movement(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void key_board_input(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
//Camera camera(glm::vec3(0.0f, 6.0f, 5.0f));
//Camera camera(glm::vec3(0.0f, 0.0f, 155.0f));
//Camera camera(glm::vec3(0.0f, 0.0f, 60.0f));//LIGHTING test
Camera* camera;// (glm::vec3(7.9019, 29.3491, 18.9233), glm::vec3(0.0f, 1.0f, 0.0f), -89.2999, -71.7001);//looking at the whole World

bool draw_world_info;

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

float current_mouse_x = lastX;
float current_mouse_y = lastY;

bool firstMouse = true;

world* World = NULL;
timing* Time = NULL;
skymap* sky = NULL;
float* deltaTime = NULL;

int main() {

    camera= new Camera(glm::vec3(7.9019, 29.3491, 18.9233), glm::vec3(0.0f, 1.0f, 0.0f), -89.2999, -71.7001);//looking at the whole World

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hallow streets", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_board_input);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);


    // glEnable(GL_BLEND);
    // glCullFace(GL_BACK);
     //glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = camera->GetViewMatrix();

    Time = new timing(false);

    deltaTime = Time->get_time_change_static();

    bool drawsky = true;
    bool sigle_light_soruce = false;

    std::cout << "creating the objects" << std::endl;

    if (drawsky) {
        sky = new skymap();
        sky->set_cam(view);
        sky->set_projection(projection);
        sky->init();
    }

    World = new world();
    World->set_time(Time);
    World->set_cam(view);
    World->set_projection(projection);
    if (sigle_light_soruce) {
        World->set_single_draw();
    }
    World->init();

    World->set_camera_obj(camera);

    while (!glfwWindowShouldClose(window))
    {
        Time->update_time();
        process_movement(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        view = camera->GetViewMatrix();

        glEnable(GL_CULL_FACE);
        World->set_cam(view);
        World->set_cam_pos(camera->get_pos());
        if (!sigle_light_soruce) {
            World->draw_deferred();
        }
        else {
            World->draw_single();
        }

        if (drawsky) {
            sky->set_cam(view);
            sky->set_projection(projection);
            sky->draw();
        }

        World->update();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    //clean up mem
    delete Time;
    if (Time != NULL) {
        delete Time;
    }
    if (World != NULL) {
        delete World;
    }
    if (sky != NULL) {
        delete sky;
    }


    glfwTerminate();
    return 0;
}

//data for the demos
#ifdef DEMO1
bool running_d1 = false;
#endif // DEMO1

void process_movement(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, *deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, *deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, *deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, *deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera->ProcessKeyboard(UP, *deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera->ProcessKeyboard(DOWN, *deltaTime);
}

void key_board_input(GLFWwindow* window, int key, int scancode, int action, int mods) {

#ifdef DEMO1
    if (!running_d1 && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        std::cout << "starting demo 1" << std::endl;
        running_d1 = true;
        World->start_demo_1();
    }
#endif // DEMO1

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //timimng changes
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
        Time->set_time_multipler(0);
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        Time->set_time_multipler(1);
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        Time->set_time_multipler(2);
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        Time->set_time_multipler(3);
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        Time->set_time_multipler(4);
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        Time->toggle_frame_rates();
    if (key == GLFW_KEY_6 && action == GLFW_RELEASE)
        World->play_sound_effect(0);
    if (key == GLFW_KEY_7 && action == GLFW_RELEASE)
        World->play_sound_effect(1);
    if (key == GLFW_KEY_8 && action == GLFW_RELEASE)
        World->play_sound_effect(2);
    if (key == GLFW_KEY_9 && action == GLFW_RELEASE)
        World->play_sound_effect(3);
    if (key == GLFW_KEY_0 && action == GLFW_RELEASE)
        World->play_sound_effect(4);

    if (key == GLFW_KEY_O && action == GLFW_RELEASE)
        World->increase_background_music();

    if (key == GLFW_KEY_L && action == GLFW_RELEASE)
        World->decrease_background_music();
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if (World != NULL) {
       float zoom = camera->ProcessMouseScroll(yoffset);
       World->change_projection(glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f));
    }
}

// glfw: whenever the mouse buttons are clicked, this callback is called
// ----------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
   
}