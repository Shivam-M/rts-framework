#pragma once

#include "moveable.h"

#include <vector>

using namespace std;

class Panel: public Moveable {
	private:
		vector<Moveable*> bundle_;

	public:
		Panel() { add_flag(PANEL); }

		void show(bool with_fade = false) {
			if (!has_flag(DISABLED)) return;
			remove_flag(DISABLED);
			for (Moveable* moveable : bundle_) moveable->remove_flag(DISABLED); // Move with fade() to prevent having to cycle twice
			if (with_fade) fade();
		}

		void hide(bool with_fade = false) {
			if (has_flag(DISABLED)) return;
			add_flag(DISABLED);
			if (with_fade) fade_out();
			else for (Moveable* moveable : bundle_) moveable->add_flag(DISABLED);
		}

		void add(Moveable* moveable) {
			moveable->set_priority(get_priority());
			moveable->add_flag(UNDER_PANEL);
			moveable->add_flag(UI);
			moveable->size *= size;
			moveable->location = location + (moveable->location * size);
			moveable->parent = this;
			bundle_.push_back(moveable);
		}

		void fade() {
			for (Moveable* mv : bundle_) {
				ColourShift entrance = ColourShift(mv->get_colour().set_alpha(0), mv->get_default_colour());
				entrance.speed = 0.0075f;
				mv->colour = mv->get_colour().set_alpha(0);
				entrance.loop = false;
				// entrance.reswap();
				entrance.direction = ColourShift::Direction::Up;
				mv->set_colour_shift(entrance);
			}

		}

		void fade_out() {
			for (Moveable* mv : bundle_) {
				// mv->set_colour(mv->get_default_colour());
				ColourShift entrance = ColourShift(mv->get_colour(), mv->get_colour().set_alpha(0));
				entrance.direction = ColourShift::Direction::Up;
				entrance.speed = 0.0075f;
				entrance.loop = false;
				entrance.fade_to_death = true;
				mv->set_colour_shift(entrance);
			}
		}

		void remove(Moveable* moveable) { bundle_.erase(std::remove(bundle_.begin(), bundle_.end(), moveable), bundle_.end()); moveable->parent = nullptr; }

		void clear() { bundle_.clear(); }

		vector<Moveable*>* get() { return &bundle_; }

        Moveable* get_moveable_by_name(const string& name) {
				for (Moveable* moveable : bundle_) {
					if (moveable->get_name() == name) {
						return moveable;
					}
				}
				return nullptr;
        }

		Text* get_text_by_name(const string& name) {
			return reinterpret_cast<Text*>(get_moveable_by_name(name));  // todo: would be safer/cleaner with a dynamic_cast, move to .cpp
		}

		void set_location(float x, float y) override {
			for (Moveable* moveable : bundle_) {
				Vector2 new_location = moveable->get_location();
				new_location.x += -get_location().x + x;
				new_location.y += -get_location().y + y;
				moveable->set_location(new_location.x, new_location.y);
			}
			Moveable::set_location(x, y);
		}

		void set_size(float x, float y) override {
			Vector2 updated_size = { x, y };
			Vector2 current_size = get_size();
			if (updated_size.x == 0 || updated_size.y == 0) return;
			for (Moveable* moveable : bundle_) {
				Vector2 original_size = moveable->size / current_size;
				Vector2 original_location = (moveable->location - get_location()) / current_size;
				moveable->size = original_size * updated_size;
				moveable->location = get_location() + (original_location * updated_size);
				/*printf("Size: %f, %f\n", moveable->size.x, moveable->size.y);
				printf("Location: %f, %f\n", moveable->location.x, moveable->location.y);*/
			}
			Moveable::set_size(x, y);
		}
};
