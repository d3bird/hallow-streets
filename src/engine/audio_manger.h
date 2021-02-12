#pragma once
#pragma comment(lib, "irrKlang.lib")

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <irrKlang/irrKlang.h>

#include "camera.h"

enum effect_names { explosion = 0, Explosion_Large_Blast_1 = 1, Explosion_Large_Blast_2 = 2, chicken_alarm_call = 3 };

struct sound{
	irrklang::ISoundSource* sound_data = NULL;
	float current_volume = 1.0f;
	float def_volume = 1.0f;
};

struct speaker {
	sound* audio;
	float max_distance;
	glm::vec3 loc;
	bool repeat;
};

class audio_manger {
public:
	audio_manger();
	~audio_manger();

	bool draw_speaker_locations() { return draw_speaker_loc; }

	void update(glm::vec3 pos, glm::vec3 look, glm::vec3 vel = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0));
	void update();

	void init();
	void play_sound_effect(int i);
	void play_background_music(int i);
	
	void increase_volume();
	void decrease_volume();

	void play_3D_sound(effect_names soun, glm::vec3 pos);

	void set_cam(Camera* c) { cam = c; }

	std::vector<glm::vec3> get_speaker_locations();

private:

	void create_sound_data();

	Camera *cam;

	float volume;
	irrklang::ISoundEngine* engine;// = createIrrKlangDevice();
	std::string audio_loc;

	std::vector<sound*> effects;
	std::vector<sound*> background_music;

	std::vector<speaker*> speaker_locs;


	bool draw_speaker_loc;
};