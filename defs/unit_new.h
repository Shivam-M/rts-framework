#pragma once

class NationNew;

#include "../assets/text_new.h"

class UnitNew: public MoveableNew {
	enum UNIT_STATE { NORMAL, SIEGING, TRAVELLING, FIGHTING, DEAD };

private:
	int identifier_ = -1;
	int size_ = 1;
	double skill_factor_ = 1.00;
	double speed_ = 0.01;
	int location_[2] = { 0, 0 };
	Vector2 _location = Vector2(0, 0);
	NationNew* nation_ = nullptr;
	ProvinceNew* province_ = nullptr;
	ProvinceNew* target_province_ = nullptr;
	vector<ProvinceNew*> travel_path_;
	UnitNew* enemy_unit_ = nullptr;
	UNIT_STATE state_ = NORMAL;

public:
	UnitNew(int identifier, NationNew* nation, int size, double skill = 1.00, ProvinceNew* province = nullptr) :
		identifier_(identifier), nation_(nation), size_(size), skill_factor_(skill) {
		setProvince(province); addFlag(TEXTURED); setTexture(Image::getImage("images/unit.png")); MoveableNew::setSize(0.075, 0.10);
	}

	void setID(int identifier) { identifier_ = identifier; }
	int getID() { return identifier_; }

	int getSize() { return size_; }
	void setSize(int size) { size_ = size; }

	double getSkill() { return skill_factor_; }
	void setSkill(double skill) { skill_factor_ = skill; }

	double getSpeed() { return speed_; }
	void setSpeed(double speed) { speed_ = speed; }

	UNIT_STATE getState() { return state_; }
	void setState(UNIT_STATE state) { state_ = state; }

	ProvinceNew* getProvince() { return province_; }
	void setProvince(ProvinceNew* province) {
		province_ = province;
		setLocation(province->getLocation().x, province->getLocation().y); // err: done before provinces are mapped with coords /// CHANGE TO CENTRE
	}

	ProvinceNew* getTarget() { return target_province_; }
	void setTarget(ProvinceNew* province) { target_province_ = province; }

	vector<ProvinceNew*> getPath() {
		return travel_path_;
	}

	void setPath(vector<ProvinceNew*> path) {
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

	void initiateBattle(UnitNew* unit) {
		setState(FIGHTING);
		enemy_unit_ = unit;
	}

	void initiate() {
		setLocation(province_->getCentre().x, province_->getCentre().y);
	}

	void evaluate() {
		// return;
		Vector2 home_location = province_->getLocation();
		Vector2 target_location = province_->getLocation();
		double magnet = 0.01;

		// target_province_->getCentre(target_location);
		home_location = province_->getCentre();
		if (target_province_) {
			target_location = target_province_->getCentre();
			// target_location[0] -= size[0] / 2;
			// target_location[1] -= size[1] / 2;
		}

		// setLocation(location_[0] / 1280.0, location_[1] / 720.0);
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

			// home_location[0] -= size[0] / 2;
			// home_location[1] -= size[1] / 2;

			location.x += (target_location.x - home_location.x) * speed_;
			location.y += (target_location.y - home_location.y) * speed_;

			distance.x = abs(target_location.x - location.x);
			distance.y = abs(target_location.y - location.y);

			magnet = 0.001;

			if (location.x == target_location.x && location.y == target_location.y || (distance.x < magnet && distance.y < magnet)) {
				province_ = target_province_;
				target_province_ = nullptr; // Get new from path

				// check if an enemy unit is also on the province
				// game_instance->checkCollision();



				if (travel_path_.size() > 1) {
					advancePath();
				}
				else {
					travel_path_.clear();
					setState(NORMAL);
				}
			}

			// location[0] -= size[0] / 2;
			// location[1] -= size[1] / 2;

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