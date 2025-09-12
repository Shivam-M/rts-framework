#include "unit.h"

#include "../game_rts.h"
#include "../assets/text.h"
#include "../assets/nation.h"
#include "../assets/province.h"
#include "../tools/image.h"
#include "../tools/common.h"

using namespace std;

Unit::Unit(int identifier, Nation* nation, int size, float skill, Province* province) :
	identifier_(identifier), nation_(nation), amount_(size), skill_factor_(skill) {
	set_province(province);
	add_flag(TEXTURED | UNIT);
	set_texture(Image::get_image("data/images/unit_thick.png"));
	Moveable::set_size(0.075f, 0.075f / 1.16f);
}

void Unit::set_path(vector<Province*> path) {
	PathCount++;
	travel_path_ = path;
	set_target_province(travel_path_.at(0));
	set_state(TRAVELLING);
	log_t("Set path for unit " CON_RED, get_name(), CON_NORMAL " (" CON_RED, get_id(), CON_NORMAL "):");

	for (Province* province : path) {
		ColourShift colourshift = ColourShift(province->get_colour(), province->get_colour().set_alpha(150));
		colourshift.speed = 0.02f;
		colourshift.conditionalise(&PathCount);
		province->set_colour_shift(colourshift);
		log_t("* " CON_RED, province->get_name(), CON_NORMAL " (" CON_RED, province->get_id(), CON_NORMAL ")");
	}
}

void Unit::advance_path() {
	travel_path_.erase(travel_path_.begin());
	if (!travel_path_.empty()) {
		set_target_province(travel_path_.at(0));
	}
}

void Unit::inflict_fatalities(int amount) {
	amount_ -= amount;
	if (amount_ <= 0) {
		amount_ = 0;
		add_flag(DISABLED);
		set_state(DEAD);
		text->add_flag(DISABLED);
		if (province_->get_besieger() == this) {
			province_->set_besieger(nullptr);
		}
	}
}

void Unit::finish_battle() {
	if (battle_info_) {
		GameRTS::instance->register_event(BATTLE_END, battle_info_);
		battle_info_ = nullptr;
		text->remove_flag(DISABLED);
	}
}

void Unit::receive_battle(Unit* unit, BattleInformation* battle_info) {
	text->add_flag(DISABLED);
	set_state(FIGHTING);
	enemy_unit_ = unit;
	battle_info_ = battle_info;
}

void Unit::initiate_battle(Unit* unit) {
	text->add_flag(DISABLED);
	set_state(FIGHTING);
	battle_info_ = new BattleInformation();
	battle_info_->attacker_units.push_back(this);
	battle_info_->defender_units.push_back(unit);
	battle_info_->total_attacker_starting_strength = amount_;
	battle_info_->total_defender_starting_strength = unit->amount_;
	battle_info_->province = province_;
	enemy_unit_ = unit;
	enemy_unit_->receive_battle(this, battle_info_);
	GameRTS::instance->register_event(BATTLE_START, battle_info_);
}

void Unit::initialise() {
	set_location(province_->get_centre().x, province_->get_centre().y);
	province_->register_unit(this);
}

void Unit::evaluate() {
	Vector2 home_location = province_->get_centre();
	Vector2 target_location;
	Vector2 distance;
	float magnet = 0.001f;

	if (target_province_) target_location = target_province_->get_centre();

	switch (get_state()) {
		case FIGHTING:
			enemy_unit_->inflict_fatalities(50 * skill_factor_ * random_float());
			if (enemy_unit_->get_state() == DEAD) {
				enemy_unit_ = nullptr;
				set_state(target_province_ ? TRAVELLING : NORMAL);
				finish_battle();
			}
			break;
		case SIEGING:
			province_->progress_siege(skill_factor_ * random_float());
			if (province_->get_siege_progress() >= 100) {
				province_->set_blend(blend_sieged);
				province_->set_gradient_colour(get_colour().set_alpha(200));
			}
			break;
		case TRAVELLING:
			location.x += (target_location.x - home_location.x) * speed_;
			location.y += (target_location.y - home_location.y) * speed_;

			distance.x = abs(target_location.x - location.x);
			distance.y = abs(target_location.y - location.y);

			if (distance.x < magnet && distance.y < magnet) {
				province_->deregister_unit(this);
				province_->stop_colour_shift();
				province_ = target_province_;
				target_province_->register_unit(this);
				target_province_ = nullptr;

				log_t("Unit " CON_RED, get_name(), CON_NORMAL " (" CON_RED, get_id(), CON_NORMAL ") arrived at province " CON_RED, province_->get_name(), CON_NORMAL " (" CON_RED, province_->get_id(), CON_NORMAL ")");

				if (!travel_path_.empty()) {
					advance_path();
				}

				if (travel_path_.empty()) {
					province_->stop_colour_shift();
					set_state(NORMAL);
				}

				const auto& units = province_->get_stationed_units();
				for (Unit* unit: units) {
					if (unit == this || unit->get_nation() == get_nation()) continue;
					if (unit->get_state() != FIGHTING && unit->get_state() != DEAD) {
						initiate_battle(unit);
						break;
					}
				}
			}
			break;
		case NORMAL:
			if (province_->get_nation() == get_nation()) break;

			if (province_->get_besieger() == nullptr || province_->get_besieger() == this) {
				province_->initiate_siege(this, get_colour());
				set_state(SIEGING);
			}
			break;
	}

	if (text) {
		Vector2 text_location = get_centre();
		text_location.y -= 0.0275f;

		text->set_content(format("{} - {}", name, amount_));
		text->set_location(text_location.x, text_location.y);
	}
}