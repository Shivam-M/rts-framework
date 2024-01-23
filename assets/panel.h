#pragma once

#include "moveable.h"

using namespace std;

class Panel: public Moveable {
	private:
		vector<Moveable*> bundle_;

	public:
		Panel() {
			addFlag(PANEL);
		}

		void show() {
			removeFlag(DISABLED);
			for (Moveable* moveable : bundle_) {
				moveable->removeFlag(DISABLED);
			}
		}

		void hide() {
			addFlag(DISABLED);
			for (Moveable* moveable : bundle_) {
				moveable->addFlag(DISABLED);
			}
		}

		void add(Moveable* moveable) {
			moveable->addFlag(UNEDITABLE);
			bundle_.push_back(moveable);
		}

		void remove(Moveable* moveable) {
			bundle_.erase(std::remove(bundle_.begin(), bundle_.end(), moveable), bundle_.end());
		}

		void clear() {
			bundle_.clear();
		}

		vector<Moveable*>* get() {
			return &bundle_;
		}

		void setLocation(float x, float y) override {
			for (Moveable* moveable : bundle_) {
				Vector2 new_loc = moveable->getLocation();
				new_loc.x += -getLocation().x + x;
				new_loc.y += -getLocation().y + y;
				moveable->setLocation(new_loc.x, new_loc.y);
			}

			Moveable::setLocation(x, y);
		}
};
