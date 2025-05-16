#pragma once

#include "moveable.h"


class ParticleGroup : public Moveable {
	private:
		Moveable template_moveable;
		vector<Moveable*> particles;
		int amount;

	public:
		ParticleGroup(Vector2 loc, Vector2 sze, Moveable moveable, int num, vector<Moveable*>* queue = nullptr) : template_moveable(moveable), amount(num) {
			addFlag(PARTICLES);
			location = loc;
			size = sze;
			particles.reserve(amount);
			queue->reserve(queue->size() + amount);
			for (int i = 0; i < amount; i++) {
				Moveable* particle = new Moveable(template_moveable);
				particle->colour_shift.speed *= random_float();
				particle->setLocation(getLocation().x + (getSize().x  * random_float()), getLocation().y + (getSize().y * random_float()));
				particles.push_back(particle);
				queue->push_back(particle);
			}
		}
		
		void update(const float& modifier) override {
			Moveable::update(modifier);
			for (auto& particle : particles) {
				// particle->update();
				if (particle->getFlags() & DISABLED) {
					// particle->setColour(template_moveable.getColour());
					particle->setLocation(getLocation().x + (getSize().x * random_float()), getLocation().y + (getSize().y * random_float()));
					particle->removeFlag(DISABLED);
				}
			}
		}
};
