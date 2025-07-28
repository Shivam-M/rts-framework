#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include "audio.h"
#include "../tools/common.h"


////////////////////////////// Audio


float Audio::volume_ = 1.0f;
int Audio::flags_ = 0;
bool Audio::enabled = true;
ma_engine Audio::engine;

void Audio::init() {
	if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
		log_t(CON_RED "ERROR! Failed to initialise miniaudio engine");
		return;
	}
	log_t("Loaded miniaudio engine successfully.");
}

void Audio::uninit() {
	ma_engine_uninit(&engine);
}

void Audio::enable() { enabled = true; }

void Audio::disable() { enabled = false; }

void Audio::stopAllSounds() {}

void Audio::setVolume(const float volume) {
	ma_engine_set_volume(&engine, volume);
	volume_ = volume;
}


////////////////////////////// Sound

// TODO: Deal with cleanup after sound finishes if it only has a one-time use (ma_sound_set_end_callback)

void Sound::load(const string file) {
	log_t("Loading sound... " CON_RED, file);
	if (ma_sound_init_from_file(&Audio::engine, file.c_str(), 0, NULL, NULL, &sound) != MA_SUCCESS) {
		log_t(CON_RED "ERROR! Failed to load sound");
		return;
	}
	file_ = file;
}

void Sound::play() {
	if (!Audio::enabled) {
		log_t("Suppressing playback of " CON_RED + file_ + CON_NORMAL + " because the audio engine is disabled.");
		return;
	}
	ma_sound_start(&sound);
}

void Sound::play(const float volume) {
	setVolume(volume);
	play();
}

void Sound::pause() {
	ma_sound_stop(&sound);
}

void Sound::stop() {
	ma_sound_seek_to_pcm_frame(&sound, 0);
}

void Sound::setVolume(const float volume) {
	ma_sound_set_volume(&sound, volume);
	volume_ = volume;
}

const float& Sound::getVolume() const {
	return volume_;
}
