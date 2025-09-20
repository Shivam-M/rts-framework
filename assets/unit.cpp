#include "unit.h"

#include "../game_rts.h"
#include "../assets/text.h"
#include "../assets/nation.h"
#include "../assets/province.h"
#include "../tools/image.h"
#include "../tools/common.h"

using namespace std;

Unit::Unit(int id, Nation* owner_nation, int size, float skill_factor, Province* starting_province) : identifier(id), nation(owner_nation), amount(size), skill(skill_factor), province(starting_province) {
	add_flag(TEXTURED | UNIT);
	set_texture(Image::get_image("data/images/unit_thick.png"));
	Moveable::set_size(0.075f, 0.075f / 1.16f);
}

void Unit::set_path(vector<Province*> path) {
	PathCount++;
	path_ = path;
	target_ = path_.at(0);
	state = State::Travelling;
	log_t("Set path for unit " CON_RED, name, CON_NORMAL " (" CON_RED, identifier, CON_NORMAL "):");

	for (Province* province : path) {
		ColourShift colourshift = ColourShift(province->colour, province->colour.with_alpha(150));
		colourshift.speed = 0.02f;
		colourshift.conditionalise(&PathCount);
		province->set_colour_shift(colourshift);
		log_t("* " CON_RED, province->name, CON_NORMAL " (" CON_RED, province->identifier, CON_NORMAL ")");
	}
}

void Unit::advance_path() {
	path_.erase(path_.begin());
	if (!path_.empty()) {
		target_ = path_.at(0);
	}
}

void Unit::inflict_fatalities(int fatalities) {
	amount -= fatalities;
	if (amount <= 0) {
		amount = 0;
		add_flag(DISABLED);
		state = State::Dead;
		text->add_flag(DISABLED);
		if (province->besieger == this) {
			province->besieger = nullptr;
		}
	}
}

void Unit::finish_battle() {
	if (battle_) {
		GameRTS::instance->register_event(BATTLE_END, battle_);
		battle_ = nullptr;
		text->remove_flag(DISABLED);
	}
}

void Unit::receive_battle(Unit* unit, BattleInformation* battle_info) {
	text->add_flag(DISABLED);
	state = State::Fighting;
	enemy_ = unit;
	battle_ = battle_info;
}

void Unit::initiate_battle(Unit* unit) {
	text->add_flag(DISABLED);
	state = State::Fighting;
	battle_ = new BattleInformation();
	battle_->attacker_units.push_back(this);
	battle_->defender_units.push_back(unit);
	battle_->total_attacker_starting_strength = amount;
	battle_->total_defender_starting_strength = unit->amount;
	battle_->province = province;
	enemy_ = unit;
	enemy_->receive_battle(this, battle_);
	GameRTS::instance->register_event(BATTLE_START, battle_);
}

void Unit::initialise() {
	set_location(province->get_centre().x, province->get_centre().y);
	province->register_unit(this);
}

void Unit::evaluate() {
	Vector2 home_location = province->get_centre();
	Vector2 target_location;
	Vector2 distance;
	float magnet = 0.001f;

	if (target_) target_location = target_->get_centre();

	switch (state) {
		case State::Fighting:
			enemy_->inflict_fatalities(50 * skill * random_float());
			if (enemy_->state == State::Dead) {
				enemy_ = nullptr;
				state = target_ ? State::Travelling : State::Normal;
				finish_battle();
			}
			break;
		case State::Sieging:
			province->progress_siege(skill * random_float());
			if (province->siege_progress >= 100) {
				province->blend = siege_lines;
				province->set_gradient_colour(colour.with_alpha(200));
			}
			break;
		case State::Travelling:
			location.x += (target_location.x - home_location.x) * speed;
			location.y += (target_location.y - home_location.y) * speed;

			distance.x = abs(target_location.x - location.x);
			distance.y = abs(target_location.y - location.y);

			if (distance.x < magnet && distance.y < magnet) {
				province->deregister_unit(this);
				province->stop_colour_shift();
				province = target_;
				target_->register_unit(this);
				target_ = nullptr;

				log_t("Unit " CON_RED, name, CON_NORMAL " (" CON_RED, identifier, CON_NORMAL ") arrived at province " CON_RED, province->name, CON_NORMAL " (" CON_RED, province->identifier, CON_NORMAL ")");

				if (!path_.empty()) {
					advance_path();
				}

				if (path_.empty()) {
					province->stop_colour_shift();
					state = State::Normal;
				}

				const auto& units = province->get_stationed_units();
				for (Unit* unit: units) {
					if (unit == this || unit->nation == nation) continue;
					if (unit->state != State::Fighting && unit->state != State::Dead) {
						initiate_battle(unit);
						break;
					}
				}
			}
			break;
		case State::Normal:
			if (province->nation == nation) break;

			if (province->besieger == nullptr || province->besieger == this) {
				province->initiate_siege(this, colour);
				state = State::Sieging;
			}
			break;
	}

	if (text) {
		Vector2 text_location = get_centre();
		text_location.y -= 0.0275f;

		text->set_content(format("{} - {}", name, amount));
		text->set_location(text_location.x, text_location.y);
	}
}