#include "skymap.h"
#include <stb_image.h>

skymap::skymap() {
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    update_projection = false;
    update_cam = false;

    shader = NULL;
    skyboxShader = NULL;

    skyboxVAO = 0;
    skyboxVBO = 0;
    cubemapTexture = 0;
}

skymap::~skymap() {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);
}
 
void skymap::draw() {

    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader->use();
    view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

void skymap::init() {
    std::cout << "creating skymap" << std::endl;
    shader = new Shader("cubemaps.vs", "cubemaps.fs");
    skyboxShader = new Shader("skybox.vs", "skybox.fs");

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> def
    {
       "resources/textures/skybox/right.jpg",
       "resources/textures/skybox/left.jpg",
       "resources/textures/skybox/top.jpg",
       "resources/textures/skybox/bottom.jpg",
       "resources/textures/skybox/front.jpg",
       "resources/textures/skybox/back.jpg"
    };
    std::vector<std::string> faces
    {
       "resources/textures/skybox/night_sky/right.png",
       "resources/textures/skybox/night_sky/left.png",
       "resources/textures/skybox/night_sky/top.png",
       "resources/textures/skybox/night_sky/bottom.png",
       "resources/textures/skybox/night_sky/front.png",
       "resources/textures/skybox/night_sky/back.png"
    };
    std::vector<std::string> nebula
    {
       "resources/textures/skybox/nebula/nebula_right.jpg",
       "resources/textures/skybox/nebula/nebula_left.jpg",
       "resources/textures/skybox/nebula/nebula_top.jpg",
       "resources/textures/skybox/nebula/nebula_bottom.jpg",
       "resources/textures/skybox/nebula/nebula_front.jpg",
       "resources/textures/skybox/nebula/nebula_back.jpg"
    };
    
    cubemapTexture = loadCubemap(faces);

    shader->use();
    shader->setInt("texture1", 0);

    skyboxShader->use();
    skyboxShader->setInt("skybox", 0);
    check();
    std::cout << "finished creating skymap" << std::endl;
}


unsigned int skymap::loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int skymap::loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void skymap::check() {

    bool errors = false;

    if (view == glm::mat4(1.0f)) {
        std::cout << "there was errors setting view" << std::endl;
        errors = true;
    }
    if (projection == glm::mat4(1.0f)) {
        std::cout << "there was errors setting projection" << std::endl;
        errors = true;
    }

   /* if (update_projection == false) {
        std::cout << "there was errors setting update_projection" << std::endl;
        errors = true;
    }
    if (update_cam == false) {
        std::cout << "there was errors setting update_cam" << std::endl;
        errors = true;
    }*/

    if (shader == NULL) {
        std::cout << "there was errors setting shader" << std::endl;
        errors = true;
    }
    if (skyboxShader == NULL) {
        std::cout << "there was errors setting skyboxShader" << std::endl;
        errors = true;
    }

    if (skyboxVAO == 0) {
        std::cout << "there was errors setting skyboxVAO" << std::endl;
        errors = true;
    }
    if (skyboxVBO == 0) {
        std::cout << "there was errors setting skyboxVBO" << std::endl;
        errors = true;
    }
    if (cubemapTexture == 0) {
        std::cout << "there was errors setting cubemapTexture" << std::endl;
        errors = true;
    }

    if (errors) {
        std::cout << "there was errors in inting the sky box" << std::endl;
        while (true);
    }
    else {
        std::cout << "passed all checks" << std::endl;
    }
}