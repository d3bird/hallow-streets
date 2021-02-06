#pragma once
#pragma comment(lib, "irrKlang.lib")

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <vector>
#include <irrKlang/irrKlang.h>

using namespace irrklang;


struct sound{
	ISoundSource* sound_data = NULL;
	float current_volume = 1.0f;
	float def_volume = 1.0f;
};



class audio_manger {
public:
	audio_manger();
	~audio_manger();


	void init();
	void play_sound_effect(int i);
	void play_background_music(int i);

private:

	void create_sound_data();

	ISoundEngine* engine;// = createIrrKlangDevice();
	std::string audio_loc;

	std::vector<sound*> effects;
	std::vector<sound*> background_music;

};