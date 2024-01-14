#pragma once

class NationNew;

#include "../assets/text.h"

class Unit: public Moveable {
	enum UNIT_STATE { NORMAL, SIEGING, TRAVELLING, FIGHTING, DEAD };

private:
	int identifier_ = -1;
	int size_ = 1;
	float skill_factor_ = 1.00;
	float speed_ = 0.01;
	int location_[2] = { 0, 0 };
	Vector2 _location = Vector2(0, 0);
	NationNew* nation_ = nullptr;
	Province* province_ = nullptr;
	Province* target_province_ = nullptr;
	vector<Province*> travel_path_;
	Unit* enemy_unit_ = nullptr;
	UNIT_STATE state_ = NORMAL;

public:
	Unit(int identifier, NationNew* nation, int size, float skill = 1.00, Province* province = nullptr) :
		identifier_(identifier), nation_(nation), size_(size), skill_factor_(skill) {
		setProvince(province); addFlag(TEXTURED | UNIT); setTexture(Image::getImage("images/unit.png")); Moveable::setSize(0.075, 0.10);
	}

	void setID(int identifier) { identifier_ = identifier; }
	int getID() { return identifier_; }

	int getSize() { return size_; }
	void setSize(int size) { size_ = size; }

	float getSkill() { return skill_factor_; }
	void setSkill(float skill) { skill_factor_ = skill; }

	float getSpeed() { return speed_; }
	void setSpeed(float speed) { speed_ = speed; }

	UNIT_STATE getState() { return state_; }
	void setState(UNIT_STATE state) { state_ = state; }

	Province* getProvince() { return province_; }
	void setProvince(Province* province) {
		province_ = province;
	}

	Province* getTarget() { return target_province_; }
	void setTarget(Province* province) { target_province_ = province; }

	vector<Province*> getPath() { return travel_path_; }

	void setPath(vector<Province*> path) {
		travel_path_ = path;
		setTarget(travel_path_.at(0));
		setState(TRAVELLING);
	}

	void advancePath() {
		travel_path_.erase(travel_path_.begin());
		if (getPath().size() != 0) {
			setTarget(travel_path_.at(0));
		}
	}

	void takeFatalities(int amount) {
		size_ -= amount;
		if (size_ <= 0) {
			setState(DEAD);
			// nation_->dismissUnit(this);
			// GameInstance.announceDeath(this);
		}
	}

	void initiateBattle(Unit* unit) {
		setState(FIGHTING);
		enemy_unit_ = unit;
	}

	void initiate() {
		setLocation(province_->getCentre().x, province_->getCentre().y);
	}

	void evaluate() {
		Vector2 home_location = province_->getCentre();
		Vector2 target_location;
		float magnet = 0.01;

		if (target_province_) {
			target_location = target_province_->getCentre();
		}

		Vector2 distance;
		switch (getState()) {
			case FIGHTING:
				enemy_unit_->takeFatalities(125 * skill_factor_ /* x randomess */);
				if (enemy_unit_->getState() == DEAD) enemy_unit_ = nullptr; // Temp: check if the enemy unit has been defeated
				if (enemy_unit_ == nullptr)
					if (target_province_) setState(TRAVELLING); // Resume travel if a target province is set
					else setState(NORMAL);
				break;
			case SIEGING:
				break;
			case TRAVELLING:
				location.x += (target_location.x - home_location.x) * speed_;
				location.y += (target_location.y - home_location.y) * speed_;

				distance.x = abs(target_location.x - location.x);
				distance.y = abs(target_location.y - location.y);

				magnet = 0.001;

				if (location.x == target_location.x && location.y == target_location.y || (distance.x < magnet && distance.y < magnet)) {
					province_ = target_province_;
					target_province_ = nullptr;

					// check if an enemy unit is also on the province

					if (travel_path_.size() > 1) {
						advancePath();
					}
					else {
						travel_path_.clear();
						setState(NORMAL);
					}
				}

				break;
			case NORMAL:
				break;
		}

		if (text != nullptr) {
			text->setLocation(location.x + text_offset.x, location.y + text_offset.y);
			text->setContent(getName() + " - " + to_string(getSize()));
		}
	}
};
