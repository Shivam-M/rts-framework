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
		static void cleanup();
		static void enable();
		static void disable();
		static void stop();
		static void set_volume(const float volume);
};


class Sound {
	private:
		ma_sound sound_{};
		float volume_ = 1.0f;
		bool playing_ = false;
		string file_ = "";

	public:
		Sound(const string& file, const float volume = 1.0f) {
			sound_ = ma_sound();
			load(file);
			set_volume(volume);
		};
		~Sound() { ma_sound_uninit(&sound_); }

		void load(const string file);
		void play();
		void play(const float volume);
		void pause();
		void stop();
		void set_volume(const float volume);
		const float& get_volume() const;

		static void on_end(void* sound, ma_sound* ma_sound) {  // todo: prime candidate for unique ptrs...
			// delete static_cast<Sound*>(sound);
		}
};
