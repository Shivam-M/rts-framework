#pragma once

#include "moveable.h"

#include "../tools/common.h"

#include <vector>

class ParticleGroup : public Moveable {
	private:
		Moveable template_moveable_;
		vector<Moveable*> particles_;
		int amount_;

	public:
		ParticleGroup(Vector2 loc, Vector2 sze, Moveable moveable, int amount, vector<Moveable*>* queue = nullptr) : template_moveable_(moveable), amount_(amount) {
			add_flag(PARTICLES);
			location = loc;
			size = sze;
			particles_.reserve(amount_);
			queue->reserve(queue->size() + amount_);
			for (int i = 0; i < amount_; i++) {
				Moveable* particle = new Moveable(template_moveable_);
				particle->colour_shift.speed = 0.05 * random_float();
				particle->set_location(location.x + (size.x  * random_float()), location.y + (size.y * random_float()));
				particles_.push_back(particle);
				queue->push_back(particle);
			}
		}
		
		void update(float modifier) override {
			Moveable::update(modifier);
			for (auto& particle : particles_) {
				// particle->update();
				if (particle->get_flags() & DISABLED) {
					*particle = template_moveable_;
					particle->colour_shift.speed = 0.05f * random_float();
					particle->set_location(location.x + (size.x * random_float()), location.y + (size.y * random_float()));
					particle->remove_flag(DISABLED);
				}
			}
		}
};
