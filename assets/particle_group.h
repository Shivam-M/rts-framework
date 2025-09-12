#pragma once

#include "moveable.h"

#include "../tools/common.h"

#include <vector>

class ParticleGroup : public Moveable {
	private:
		Moveable template_moveable;
		vector<Moveable*> particles;
		int amount;

	public:
		ParticleGroup(Vector2 loc, Vector2 sze, Moveable moveable, int num, vector<Moveable*>* queue = nullptr) : template_moveable(moveable), amount(num) {
			add_flag(PARTICLES);
			location = loc;
			size = sze;
			particles.reserve(amount);
			queue->reserve(queue->size() + amount);
			for (int i = 0; i < amount; i++) {
				Moveable* particle = new Moveable(template_moveable);
				particle->colour_shift.speed *= random_float();
				particle->set_location(get_location().x + (get_size().x  * random_float()), get_location().y + (get_size().y * random_float()));
				particles.push_back(particle);
				queue->push_back(particle);
			}
		}
		
		void update(const float& modifier) override {
			Moveable::update(modifier);
			for (auto& particle : particles) {
				// particle->update();
				if (particle->get_flags() & DISABLED) {
					// particle->set_colour(template_moveable.get_colour());
					particle->set_location(get_location().x + (get_size().x * random_float()), get_location().y + (get_size().y * random_float()));
					particle->remove_flag(DISABLED);
				}
			}
		}
};
