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
	setProvince(province);
	addFlag(TEXTURED | UNIT);
	setTexture(Image::getImage("data/images/unit_thick.png"));
	Moveable::setSize(0.075f, 0.075f / 1.16f);
}

void Unit::setPath(vector<Province*> path) {
	PathCount++;
	travel_path_ = path;
	setTarget(travel_path_.at(0));
	setState(TRAVELLING);
	log_t("Set path for unit " CON_RED, getName(), CON_NORMAL " (" CON_RED, getID(), CON_NORMAL "):");

	for (Province* province : path) {
		ColourShift colourshift = ColourShift(province->getColour(), province->getColour().setW(150));
		colourshift.speed = 0.02f;
		colourshift.setCondition(&PathCount);
		province->setColourShift(colourshift);
		log_t("* " CON_RED, province->getName(), CON_NORMAL " (" CON_RED, province->getID(), CON_NORMAL ")");
	}
}

void Unit::advancePath() {
	travel_path_.erase(travel_path_.begin());
	if (!travel_path_.empty()) {
		setTarget(travel_path_.at(0));
	}
}

void Unit::takeFatalities(int amount) {
	amount_ -= amount;
	if (amount_ <= 0) {
		amount_ = 0;
		addFlag(DISABLED);
		setState(DEAD);
		text->addFlag(DISABLED);
	}
}

void Unit::endBattle() {
	if (battle_info_) {
		GameRTS::instance->registerEvent(BATTLE_END, battle_info_);
		battle_info_ = nullptr;
		text->removeFlag(DISABLED);
	}
}

void Unit::receiveBattle(Unit* unit, BattleInformation* battle_info) {
	text->addFlag(DISABLED);
	setState(FIGHTING);
	enemy_unit_ = unit;
	battle_info_ = battle_info;
}

void Unit::initiateBattle(Unit* unit) {
	text->addFlag(DISABLED);
	setState(FIGHTING);
	battle_info_ = new BattleInformation();
	battle_info_->attacker_units.push_back(this);
	battle_info_->defender_units.push_back(unit);
	battle_info_->total_attacker_starting_strength = amount_;
	battle_info_->total_defender_starting_strength = unit->amount_;
	battle_info_->province = province_;
	enemy_unit_ = unit;
	enemy_unit_->receiveBattle(this, battle_info_);
	GameRTS::instance->registerEvent(BATTLE_START, battle_info_);
}

void Unit::initiate() {
	setLocation(province_->getCentre().x, province_->getCentre().y);
	province_->registerUnit(this);
}

void Unit::evaluate() {
	Vector2 home_location = province_->getCentre();
	Vector2 target_location;
	Vector2 distance;
	float magnet = 0.001f;

	if (target_province_) target_location = target_province_->getCentre();

	switch (getState()) {
		case FIGHTING:
			enemy_unit_->takeFatalities(50 * skill_factor_ * random_float());
			if (enemy_unit_->getState() == DEAD) {
				enemy_unit_ = nullptr;
				setState(target_province_ ? TRAVELLING : NORMAL);
				endBattle();
			}
			break;
		case SIEGING:
			province_->progressSiege(skill_factor_ * random_float());
			if (province_->getSiegeProgress() >= 100) {
				province_->setBlend(blend_sieged);
				province_->setGradientColour(getColour().setW(200));
			}
			break;
		case TRAVELLING:
			location.x += (target_location.x - home_location.x) * speed_;
			location.y += (target_location.y - home_location.y) * speed_;

			distance.x = abs(target_location.x - location.x);
			distance.y = abs(target_location.y - location.y);

			if (distance.x < magnet && distance.y < magnet) {
				province_->deregisterUnit(this);
				province_->stopColourShift();
				province_ = target_province_;
				target_province_->registerUnit(this);
				target_province_ = nullptr;

				log_t("Unit " CON_RED, getName(), CON_NORMAL " (" CON_RED, getID(), CON_NORMAL ") arrived at province " CON_RED, province_->getName(), CON_NORMAL " (" CON_RED, province_->getID(), CON_NORMAL ")");

				if (!travel_path_.empty()) {
					advancePath();
				}

				if (travel_path_.empty()) {
					province_->stopColourShift();
					setState(NORMAL);
				}

				const auto& units = province_->getStationedUnits();
				for (Unit* unit: units) {
					if (unit == this || unit->getNation() == getNation()) continue;
					if (unit->getState() != FIGHTING && unit->getState() != DEAD) {
						initiateBattle(unit);
						break;
					}
				}
			}
			break;
		case NORMAL:
			if (province_->getNation() == getNation()) break;

			if (province_->getBesieger() == nullptr || province_->getBesieger() == this) {
				province_->beginSiege(this, getColour());
				setState(SIEGING);
			}
			break;
	}

	if (text) {
		Vector2 text_location = getCentre();
		text_location.y -= 0.0275f;

		text->setContent(format("{} - {}", name, amount_));
		text->setLocation(text_location.x, text_location.y);
	}
}