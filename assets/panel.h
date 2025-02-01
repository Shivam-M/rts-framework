#pragma once

#include "moveable.h"


class Panel: public Moveable {
	private:
		vector<Moveable*> bundle_;

	public:
		Panel() { addFlag(PANEL); }

		void show(bool with_fade = false) {
			removeFlag(DISABLED);
			for (Moveable* moveable : bundle_) moveable->removeFlag(DISABLED); // Move with fade() to prevent having to cycle twice
			if (with_fade) fade();
		}

		void hide(bool with_fade = false) {
			addFlag(DISABLED);
			if (with_fade) fade_out();
			else for (Moveable* moveable : bundle_) moveable->addFlag(DISABLED);
		}

		void add(Moveable* moveable) {
			moveable->setPriority(getPriority());
			moveable->addFlag(UNDER_PANEL);
			moveable->size = moveable->size * getSize();
			moveable->location = getLocation() + (moveable->location * getSize());
			moveable->parent = this;
			bundle_.push_back(moveable);
		}

		void fade() {
			for (Moveable* mv : bundle_) {
				ColourShift entrance = ColourShift(mv->getColour().setW(0), mv->getDefaultColour());
				entrance.speed = 0.0075f;
				mv->colour = mv->getColour().setW(0);
				entrance.loop = false;
				entrance.reswap();
				entrance.direction = ColourShift::DIRECTION::DOWN;
				mv->setColourShift(entrance);
			}

		}

		void fade_out() {
			for (Moveable* mv : bundle_) {
				mv->setColour(mv->getDefaultColour());
				ColourShift entrance = ColourShift(mv->getColour(), mv->getColour().setW(0));
				entrance.speed = 0.0075f;
				entrance.loop = false;
				entrance.fade_to_death = true;
				mv->setColourShift(entrance);
			}
		}

		void remove(Moveable* moveable) { bundle_.erase(std::remove(bundle_.begin(), bundle_.end(), moveable), bundle_.end()); moveable->parent = nullptr; }

		void clear() { bundle_.clear(); }

		vector<Moveable*>* get() { return &bundle_; }

		void setLocation(float x, float y) override {
			for (Moveable* moveable : bundle_) {
				Vector2 new_location = moveable->getLocation();
				new_location.x += -getLocation().x + x;
				new_location.y += -getLocation().y + y;
				moveable->setLocation(new_location.x, new_location.y);
			}
			Moveable::setLocation(x, y);
		}

		void setSize(float x, float y) override {
			Vector2 updated_size = { x, y };
			Vector2 current_size = getSize();
			for (Moveable* moveable : bundle_) {
				Vector2 original_size = moveable->size / current_size;
				Vector2 original_location = (moveable->location - getLocation()) / current_size;
				moveable->size = original_size * updated_size;
				moveable->location = getLocation() + (original_location * updated_size);
			}
			Moveable::setSize(x, y);
		}
};
