#pragma once

#include <miniaudio.h>
#include <string>

using namespace std;

class Audio {
	private:
		static float volume_;
		static int flags_;
			
	public:
		static ma_engine engine;
		static bool enabled;

		static void init();
		static void uninit();
		static void enable();
		static void disable();
		static void stopAllSounds();
		static void setVolume(const float volume);
};


class Sound {
	private:
		ma_sound sound{};
		float volume_ = 1.0f;
		bool playing_ = false;
		string file_ = "";

	public:
		Sound(const string& file, const float volume = 1.0f) {
			sound = ma_sound();
			load(file);
			setVolume(volume);
		};
		~Sound() { if (&sound) ma_sound_uninit(&sound); }

		void load(const string file);
		void play();
		void play(const float volume);
		void pause();
		void stop();
		void setVolume(const float volume);
		const float& getVolume() const;
};
