#pragma once

#include "moveable.h"

#include <vector>

using namespace std;

class Panel: public Moveable {
	private:
		vector<Moveable*> bundle_;

	public:
		Panel();

		void show(bool with_fade = false);
		void hide(bool with_fade = false);
		void add(Moveable* moveable);
		void fade();
		void fade_out();
		void remove(Moveable* moveable);
		void clear();

		void set_location(float x, float y) override;
		void set_size(float x, float y) override;

		vector<Moveable*>* get();
		Moveable* get_moveable_by_name(const string& name);
		Text* get_text_by_name(const string& name);
};
