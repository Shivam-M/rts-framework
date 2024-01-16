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
			if (this->getFlags() & DISABLED) removeFlag(DISABLED);
		}

		void hide() {
			addFlag(DISABLED);
		}

		void add(Moveable* moveable) {
			bundle_.push_back(moveable);
		}

		void remove(Moveable* moveable) {
			bundle_.erase(std::remove(bundle_.begin(), bundle_.end(), moveable), bundle_.end());
		}

		void clear() {
			bundle_.clear();
		}

		vector<Moveable*> get() {
			return bundle_;
		}
};
