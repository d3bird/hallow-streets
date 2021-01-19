#include "world.h"


world::world(){
	single = false;
    draw_lights_debug = false;
    render_text = true;
}

world::~world(){
	delete City;
}

//draw with a single light source
void world::draw_single() {

	//std::cout << "drawling sky" << std::endl;
	Sky->set_cam(view);
	Sky->draw();

	//std::cout << "setting lighting " << std::endl;
	lighting_in->use();
	lighting_in->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	//lighting_in->setVec3("lightPos", Sky->get_light_loc());
	lighting_in->setVec3("lightPos", glm::vec3(10,10,10));
	//lighting_in->setVec3("lightColor", 0.0f, 1.0f, 1.0f);
	lighting_in->setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	//std::cout << "drawling city" << std::endl;
	City->set_cam(view);
	City->draw();

}

//draw with deferred shadering
void world::draw_deferred() {

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    shaderGeometryPass->use();
    shaderGeometryPass->setMat4("projection", projection);
    shaderGeometryPass->setMat4("view", view);

    //draw the entire scene
    City->set_cam(view);
    City->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //  std::cout << "lighting tests" << std::endl;

    // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    // -----------------------------------------------------------------------------------------------------------------------
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderLightingPass->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    // send light relevant uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++)
    {
        shaderLightingPass->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        shaderLightingPass->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
        // update attenuation parameters and calculate radius
         float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
         float linear = 0.22;
         float quadratic = 0.2;
        shaderLightingPass->setFloat("lights[" + std::to_string(i) + "].Linear", linear);
        shaderLightingPass->setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
        float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
        shaderLightingPass->setFloat("lights[" + std::to_string(i) + "].Radius", radius);
    }

    shaderLightingPass->setVec3("viewPos", cam_pos);
    // finally render quad
    renderQuad();

    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    // ----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. render lights on top of scene
    // --------------------------------
    if (draw_lights_debug) {
        shaderLightBox->use();
        shaderLightBox->setMat4("projection", projection);
        shaderLightBox->setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            shaderLightBox->setMat4("model", model);
            shaderLightBox->setVec3("lightColor", lightColors[i]);
            renderCube();
        }
    }

    //daw objects that need to be affected by blending
    if (render_text) {
        glEnable(GL_BLEND);
        text_render->draw();
        glDisable(GL_BLEND);
    }
}

void world::update() {
	Sky->update();
}

void world::change_projection(glm::mat4 i) {
    projection = i;
    City->set_projection(projection);
    Sky->set_projection(projection);
}

void world::init() {

	lighting_in = new Shader("lighting_instance.vs", "lighting_instance.fs");

	lighting_init();

    if (render_text) {
        text_render = new text_engine();
        text_render->set_time(Time);
        text_render->set_projection(projection);
        text_render->set_cam(view);
        text_render->init();
    }

	City = new city();
	City->set_time(Time);
	City->set_projection(projection);
	City->set_cam(view);
    if (single) {
        City->set_shader(lighting_in);
    }
    else {
        City->set_shader(shaderGeometryPass);
    }
	City->init();

	Sky = new sky();
	Sky->set_projection(projection);
	Sky->set_cam(view);
	Sky->set_width(10, 10, 10);
	Sky->set_time(Time);
	Sky->init();

	Sky->pause_time_at_noon();

	std::cout << "finished initing objects" << std::endl;
	std::cout << std::endl;
}

void world::lighting_init() {

    shaderGeometryPass = new Shader("g_buffer.vs", "g_buffer.fs");
    shaderLightingPass = new Shader("deferred_shading.vs", "deferred_shading.fs");
    shaderLightBox = new Shader("deferred_light_box.vs", "deferred_light_box.fs");

    // load models
    // -----------

    glm::mat4 temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(-3.0, -0.5, -3.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(0.0, -0.5, -3.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(3.0, -0.5, -3.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(-3.0, -0.5, 0.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(0.0, -0.5, 0.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(3.0, -0.5, 0.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(-3.0, -0.5, 3.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(0.0, -0.5, 3.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);

    temp_mod = glm::mat4(1.0);
    temp_mod = glm::translate(temp_mod, glm::vec3(3.0, -0.5, 3.0));
    temp_mod = glm::scale(temp_mod, glm::vec3(0.25f));
    objectPositions.push_back(temp_mod);


    modelMatrices = new glm::mat4[objectPositions.size()];

    for (int i = 0; i < objectPositions.size(); i++) {
        modelMatrices[i] = objectPositions[i];
    }

    backpack = new Model(("resources/objects/planet/planet.obj"));


    glGenBuffers(1, &mod_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mod_buffer);
    glBufferData(GL_ARRAY_BUFFER, objectPositions.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < backpack->meshes.size(); i++)
    {
        unsigned int VAO = backpack->meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    // configure g-buffer framebuffer
    // ------------------------------

    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 

    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------

    srand(13);
    int x = 1;
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = x;
        x += 8;
        float yPos = 4;
        float zPos = 1;
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        float rColor;
        float gColor;
        float bColor;
        if (i == 0) {
            rColor = 0;
            gColor = 0;
            bColor = 1;

        }
        else {
            rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
            gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
            bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        }

        // also calculate random color

        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }

    // shader configuration
    // --------------------
    shaderLightingPass->use();
    shaderLightingPass->setInt("gPosition", 0);
    shaderLightingPass->setInt("gNormal", 1);
    shaderLightingPass->setInt("gAlbedoSpec", 2);


}


void world::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void world::renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

