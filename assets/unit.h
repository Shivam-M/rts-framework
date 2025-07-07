#pragma once

#include "../assets/text.h"
#include "../game_rts.h"

class Nation;

static int PathCount = 0;

class Unit: public Moveable {
	private:
		int identifier_ = -1;
		int amount_ = 1;
		float skill_factor_ = 1.00f;
		float speed_ = 0.01f;
		Nation* nation_ = nullptr;
		Province* province_ = nullptr;
		Province* target_province_ = nullptr;
		vector<Province*> travel_path_;
		Unit* enemy_unit_ = nullptr;
		Blend blend_sieged = Blend(2, 1.0f, 1.0f, Vector2(-1.f, -1.5f), false);
		BattleInformation* battle_info_ = nullptr;

	public:
		enum UNIT_STATE { NORMAL, SIEGING, TRAVELLING, FIGHTING, DEAD };
		UNIT_STATE state_ = NORMAL;

		Unit(int identifier, Nation* nation, int size, float skill = 1.00f, Province* province = nullptr) :
			identifier_(identifier), nation_(nation), amount_(size), skill_factor_(skill) {
			setProvince(province); addFlag(TEXTURED | UNIT); setTexture(Image::getImage("data/images/unit_thick.png")); Moveable::setSize(0.075f, 0.075f / 1.16f);
		}

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		int getAmount() { return amount_; }
		void setAmount(int amount) { amount_ = amount; }

		float getSkill() { return skill_factor_; }
		void setSkill(float skill) { skill_factor_ = skill; }

		float getSpeed() { return speed_; }
		void setSpeed(float speed) { speed_ = speed; }

		UNIT_STATE getState() { return state_; }
		void setState(UNIT_STATE state) { state_ = state; }

		Province* getProvince() { return province_; }
		void setProvince(Province* province) { province_ = province; }

		Province* getTarget() { return target_province_; }
		void setTarget(Province* province) { target_province_ = province; }

		vector<Province*> getPath() { return travel_path_; }

		void setPath(vector<Province*> path) {
			PathCount++;
			travel_path_ = path;
			setTarget(travel_path_.at(0));
			setState(TRAVELLING);
			log_t("Set path for unit " CON_RED + getName() + CON_NORMAL " (" CON_RED, getID(), CON_NORMAL "):");

			for (Province* province : path) {
				ColourShift colourshift = ColourShift(province->getColour(), province->getColour().setW(150));
				colourshift.speed = 0.02f;
				colourshift.setCondition(&PathCount);
				province->setColourShift(colourshift);
				log_t("* " CON_RED, province->getName(), CON_NORMAL " (" CON_RED, province->getID(), CON_NORMAL ")"); 
			}
		}

		void advancePath() {
			travel_path_.erase(travel_path_.begin());
			if (!travel_path_.empty()) {
				setTarget(travel_path_.at(0));
			}
		}

		void takeFatalities(int amount) {
			amount_ -= amount;
			if (amount_ <= 0) {
				amount_ = 0;
				addFlag(DISABLED);
				setState(DEAD);
				text->addFlag(DISABLED);
				// nation_->dismissUnit(this);
				// GameInstance.announceDeath(this);
			}
		}

		void receiveBattle(Unit* unit, BattleInformation* battle_info) {
			setState(FIGHTING);
			enemy_unit_ = unit;
			battle_info_ = battle_info;
		}

		void initiateBattle(Unit* unit) {
			setState(FIGHTING);
			BattleInformation* battle_information = new BattleInformation();
			battle_information->attacker_units.push_back(this);
			battle_information->defender_units.push_back(unit);
			battle_information->total_attacker_starting_strength = amount_;
			battle_information->total_defender_starting_strength = unit->amount_;
			battle_info_ = battle_information;
			
			enemy_unit_ = unit;
			enemy_unit_->receiveBattle(this, battle_info_);
			GameRTS::instance->registerEvent(START_BATTLE, battle_information);
			/*BattleInformation battle_information = BattleInformation();
			battle_information.attacker_units.push_back(this);
			battle_information.attacker_units.push_back(enemy_unit_);
			battle_information.total_attacker_starting_strength = amount_;
			battle_information.total_defender_starting_strength = enemy_unit_->amount_;
			UIManager::AssignValues("ui_battle_unit", &battle_information);*/
		}

		void initiate() {
			setLocation(province_->getCentre().x, province_->getCentre().y);
			province_->registerUnit(this);
		}

		Nation* getNation() { return nation_; }
		void setNation(Nation* nation) { nation_ = nation; }

		void evaluate() {
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
					}
					break;
				case SIEGING:
					province_->progressSiege(skill_factor_ * random_float());
					if (province_->getSiegeProgress() >= 100) {
						// province_->setSecondaryTexture(Image::getImage("data/images/siege_lines.png"));
						province_->setBlend(blend_sieged);
						province_->setGradientColour(getColour().setW(200));
						// province_->setGradientColour(province_->getColour());
						//setState(NORMAL);
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
						province_->beginSiege(this, getColour()); // TODO: Mercenaries may be different colour
						setState(SIEGING);
					}
					break;
			}

			if (text) {
				Vector2 text_location = getCentre();
				Vector2 text_size = text->getTextSize();

				text_size.x /= WINDOW_WIDTH;
				text_size.y /= WINDOW_HEIGHT;
				text_location.y -= 0.0275f;
				text_location.x -= text_size.x / 2;

				// text->setLocation(location.x + text_offset.x, location.y + text_offset.y);
				text->setContent(format("{} - {}", name, amount_));
				text->setLocation(text_location.x, text_location.y);
			}
		}
};
