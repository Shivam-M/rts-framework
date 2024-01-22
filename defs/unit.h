#pragma once

class Nation;

#include "../assets/text.h"

enum UNIT_STATE { NORMAL, SIEGING, TRAVELLING, FIGHTING, DEAD };

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
		UNIT_STATE state_ = NORMAL;

	public:
		Unit(int identifier, Nation* nation, int size, float skill = 1.00, Province* province = nullptr) :
			identifier_(identifier), nation_(nation), amount_(size), skill_factor_(skill) {
			setProvince(province); addFlag(TEXTURED | UNIT); setTexture(Image::getImage("images/unit_thick.png")); Moveable::setSize(0.075, 0.075 / 1.16);
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
			travel_path_ = path;
			setTarget(travel_path_.at(0));
			setState(TRAVELLING);
			log_t("Set path for unit " CON_RED + getName() + CON_NORMAL " (" CON_RED, getID(), CON_NORMAL "):");
			PathCount++;

			for (Province* province : path) {
				Colour col = province->getColour();
				col.setW(150);
				ColourShift colourshift = ColourShift(province->getColour(), col);
				colourshift.speed = 0.02f;
				colourshift.setCondition(&PathCount);
				province->setColourShift(colourshift);
				log_t("\t* " CON_RED, province->getName(), CON_NORMAL " (" CON_RED, province->getID(), CON_NORMAL ")"); 
			}
		}

		void advancePath() {
			travel_path_.erase(travel_path_.begin());
			if (getPath().size() != 0) {
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

		void receiveBattle(Unit* unit) {
			setState(FIGHTING);
			enemy_unit_ = unit;
		}

		void initiateBattle(Unit* unit) {
			setState(FIGHTING);
			enemy_unit_ = unit;
			enemy_unit_->receiveBattle(this);
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
			float magnet = 0.001f;

			if (target_province_) target_location = target_province_->getCentre();

			Vector2 distance;
			switch (getState()) {
				case FIGHTING:
					enemy_unit_->takeFatalities(50 * skill_factor_ * random_float());
					if (enemy_unit_->getState() == DEAD) {
						enemy_unit_ = nullptr;
						setState(target_province_ ? TRAVELLING : NORMAL);
					}
					break;
				case SIEGING:
					break;
				case TRAVELLING:
					location.x += (target_location.x - home_location.x) * speed_;
					location.y += (target_location.y - home_location.y) * speed_;

					distance.x = abs(target_location.x - location.x);
					distance.y = abs(target_location.y - location.y);

					if (location.x == target_location.x && location.y == target_location.y || (distance.x < magnet && distance.y < magnet)) {
						province_->deregisterUnit(this);
						province_->stopColourShift();
						target_province_->registerUnit(this);
						province_ = target_province_;
						target_province_ = nullptr;

						log_t("Unit " CON_RED, getName(), CON_NORMAL " (" CON_RED, getID(), CON_NORMAL ") arrived at province " CON_RED, province_->getName(), CON_NORMAL " (" CON_RED, province_->getID(), CON_NORMAL ")");

						if (travel_path_.size() > 1) {
							advancePath();
						} else {
							province_->stopColourShift();
							travel_path_.clear();
							setState(NORMAL);
						}

						vector<Unit*> units = province_->getStationedUnits();
						if (units.size() > 1) {
							for (Unit* unit: units) {
								if (unit == this) continue;
								if (unit->getNation() != getNation()) {
									if (unit->getState() != FIGHTING && unit->getState() != DEAD) {
										initiateBattle(unit);
										break;
									}
								}
							}
						}
					}

					break;
				case NORMAL:
					break;
			}

			if (text != nullptr) {
				text->setContent(getName() + " - " + to_string(getAmount()));

				Vector2 text_location = getCentre();
				Vector2 text_size = text->getTextSize();

				text_size.x /= 1280;
				text_size.y /= 720;
				text_location.y -= 0.0275;
				text_location.x -= text_size.x / 2;

				// text->setLocation(location.x + text_offset.x, location.y + text_offset.y);
				text->setLocation(text_location.x, text_location.y);
			}
		}
};
