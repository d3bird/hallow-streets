#include "audio_manger.h"

audio_manger::audio_manger() {
	draw_speaker_loc = true;
	cam = NULL;
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


void audio_manger::update(glm::vec3 pos, glm::vec3 look, glm::vec3 vel, glm::vec3 up) {
	engine->setListenerPosition(irrklang::vec3df(pos.x,pos.y,pos.z), irrklang::vec3df(look.x, look.y, look.z),
		irrklang::vec3df(vel.x, vel.y, vel.z), irrklang::vec3df(up.x, up.y, up.z));
}

void audio_manger::update() {
	if (cam == NULL) {
		std::cout << "cammera has not been linked. can not get listener possiture from the cammera"<< std::endl;
	}
	else {
		glm::vec3 pos = cam->get_pos();
		glm::vec3 look = cam->get_look();
			//glm::vec3 vel, glm::vec3 up
		engine->setListenerPosition(irrklang::vec3df(pos.x, pos.y, pos.z), irrklang::vec3df(look.x, look.y, look.z));
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
		engine->play2D(background_music[i]->sound_data);
	}
}


void audio_manger::init() {
	std::cout << "creating the audio engine" << std::endl;

	engine = irrklang::createIrrKlangDevice();
	
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
	//temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/unused_audio/Explosion_Large_Blast_1.mp3");
	//temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/unused_audio/Explosion_Large_Blast_2.mp3");
	temp_sound_data->sound_data->setDefaultVolume();
	effects.push_back(temp_sound_data);

	speaker* newspeaker = new speaker;
	newspeaker->audio = temp_sound_data;
	newspeaker->loc = glm::vec3(5.0, 5.0, 4.0);
	newspeaker->repeat = true;
	speaker_locs.push_back(newspeaker);
	irrklang::vec3df temp_loc(newspeaker->loc.x, newspeaker->loc.y, newspeaker->loc.z);

	engine->play3D(newspeaker->audio->sound_data, temp_loc, true);

}


std::vector<glm::vec3> audio_manger::get_speaker_locations() {
	std::vector<glm::vec3> output;

	for (int i = 0; i < speaker_locs.size(); i++) {
		output.push_back(speaker_locs[i]->loc);
	}

	return output;
}
