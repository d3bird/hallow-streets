#include "audio_manger.h"

audio_manger::audio_manger() {
	draw_speaker_loc = true;
	cam = NULL;
	volume = 1.0f;
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
	engine->setListenerPosition(irrklang::vec3df( pos.x, pos.y,  pos.z), irrklang::vec3df(-1 * look.x, -1 * look.y, -1 * look.z),
		irrklang::vec3df(vel.x, vel.y, vel.z), irrklang::vec3df(up.x, up.y, up.z));
}

void audio_manger::update() {
	if (cam == NULL) {
		std::cout << "cammera has not been linked. can not get listener possiture from the cammera"<< std::endl;
	}
	else {
		glm::vec3 pos = cam->get_pos();
		glm::vec3 look = cam->get_look();
		glm::vec3 vel = glm::vec3(0, 0, 0);
		glm::vec3 up = cam->get_up();
		engine->setListenerPosition(irrklang::vec3df(pos.x, pos.y, pos.z), irrklang::vec3df(-1*look.x, -1 * look.y, -1 * look.z),
			irrklang::vec3df(vel.x, vel.y, vel.z), irrklang::vec3df(up.x, up.y, up.z));
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

void audio_manger::play_3D_sound(effect_names soun, glm::vec3 pos) {
	int index = 0;
	switch (soun)
	{
	case explosion:
		index = 0;
		break;
	case Explosion_Large_Blast_1:
		index = 1;
		break;
	case Explosion_Large_Blast_2:
		index = 2;
		break;
	case chicken_alarm_call:
		index = 3;
		break;
	default:
		index = -1;
		break;
	}

	irrklang::vec3df temp_loc(pos.x, pos.y, pos.z);

	engine->play3D(effects[3]->sound_data, temp_loc);

}


void audio_manger::create_sound_data() {
	sound* temp_sound_data;

	//create the sound efffects
	temp_sound_data = new sound;
	temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/effects/explosion.wav");
	if (temp_sound_data->sound_data == 0) {
		std::cout << "there was a problem importing sound 0" << std::endl;
	}
	else {
		temp_sound_data->sound_data->setDefaultVolume();
		effects.push_back(temp_sound_data);
	}
	temp_sound_data = new sound;
	temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/unused_audio/Explosion_Large_Blast_1.mp3");
	if (temp_sound_data->sound_data == 0) {
		std::cout << "there was a problem importing sound 1" << std::endl;
	}
	else {
		temp_sound_data->sound_data->setDefaultVolume(0.5f);
		temp_sound_data->sound_data->setDefaultMaxDistance(400.0f);
		effects.push_back(temp_sound_data);
	}

	temp_sound_data = new sound;
	temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/unused_audio/Explosion_Large_Blast_2.mp3");
	if (temp_sound_data->sound_data == 0) {
		std::cout << "there was a problem importing sound 2" << std::endl;
	}
	else {
		temp_sound_data->sound_data->setDefaultVolume(0.5f);
		effects.push_back(temp_sound_data);
	}

	temp_sound_data = new sound;
	temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/effects/chicken-alarm-call.wav");
	if (temp_sound_data->sound_data == 0) {
		std::cout << "there was a problem importing sound 2" << std::endl;
	}
	else {
		temp_sound_data->sound_data->setDefaultVolume(5.0f);
		temp_sound_data->sound_data->setDefaultMaxDistance(30.0f);
		effects.push_back(temp_sound_data);
	}


	//create the backgorund music

	temp_sound_data = new sound;
	temp_sound_data->sound_data = engine->addSoundSourceFromFile("resources/audio/background_music/space.ogg");
	if (temp_sound_data->sound_data == 0) {
		std::cout << "there was a problem importing space background loop" << std::endl;
	}
	else {
		temp_sound_data->sound_data->setDefaultVolume(0.1f);
		background_music.push_back(temp_sound_data);
	}


	//start the background music
	engine->play2D(temp_sound_data->sound_data, true);

	//create the speakers

	speaker* newspeaker = new speaker;
	newspeaker->audio = effects[0];
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


void audio_manger::increase_volume() {
	volume += 0.1;
	if (volume > 1.0f) {
		volume = 1.0f;
	}
	std::cout << "volume is " << volume << std::endl;
	engine->setSoundVolume(volume);
}

void audio_manger::decrease_volume() {
	volume -= 0.1;
	if (volume < 0.0f) {
		volume = 0.0f;
	}
	std::cout << "volume is " << volume << std::endl;
	engine->setSoundVolume(volume);

}