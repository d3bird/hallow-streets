#include "audio_manger.h"

audio_manger::audio_manger() {


}

audio_manger::~audio_manger() {
	//remove the sound effects
	for (int i = effects.size() -1; i >= 0; i--) {
		effects[i]->sound_data->drop();
		delete effects[i]->sound_data;
		delete effects[i];
	}

	//remove the background music
	for (int i = background_music.size() - 1; i >= 0; i--) {
		background_music[i]->sound_data->drop();
		delete background_music[i]->sound_data;
		delete background_music[i];
	}

	//destroy the engine
	if (engine != NULL) {
		engine->drop();
	}
}


void audio_manger::play_sound_effect(int i) {
	if (i >= effects.size()) {
		std::cout << "sound effect not found,  size = "<< effects.size()<<" entered "<<i << std::endl;
	}
	else {
		engine->play2D(effects[i]->sound_data);
	}
}

void audio_manger::play_background_music(int i) {
	if (i >= background_music.size()) {
		std::cout << "background music track not found" << std::endl;
	}
	else {

	}
}


void audio_manger::init() {
	std::cout << "creating the audio engine" << std::endl;

	engine = createIrrKlangDevice();
	
	audio_loc = "resources/audio/effects/";

	if (!engine) {
		std::cout << "ISoundEngine did not build right" << std::endl;
		return;
	}

	create_sound_data();

	std::cout << "finished creating the audio engine" << std::endl;
}

void audio_manger::create_sound_data() {
	sound* temp_sound_data;

	//create teh explosion sound efffect
	temp_sound_data = new sound;
	temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/effects/explosion.wav");
	effects.push_back(temp_sound_data);

}