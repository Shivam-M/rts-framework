#pragma once

class Nation;

class Unit: public Moveable {
	enum UNIT_STATE { NORMAL, SIEGING, TRAVELLING, FIGHTING, DEAD};

	private:
		int identifier_ = -1;
		int size_ = 1;
		double skill_factor_ = 1.00;
		double speed_ = 0.01;
		int location_[2] = { 0, 0 };
		Vector2 _location = Vector2(0, 0);
		Nation* nation_ = nullptr;
		ProvinceMV* province_ = nullptr;
		ProvinceMV* target_province_ = nullptr;
		vector<ProvinceMV*> travel_path_;
		Unit* enemy_unit_ = nullptr;
		UNIT_STATE state_ = NORMAL;

	public:
		Unit(int identifier, Nation* nation, int size, double skill = 1.00, ProvinceMV* province = nullptr) :
			identifier_(identifier), nation_(nation), size_(size), skill_factor_(skill) {
			setProvince(province); addFlag(TEXTURED); setTexture(Image::getImage("images/unit.png")); Moveable::setSize(0.075, 0.10);
		}

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		int getSize() { return size_; }
		void setSize(int size) { size_ = size; }

		double getSkill() { return skill_factor_; }
		void setSkill(double skill) { skill_factor_ = skill; }

		double getSpeed() { return speed_; }
		void setSpeed(double speed) { speed_ = speed; }

		double getX() { return location_[0]; }
		double getY() { return location_[1]; }

		UNIT_STATE getState() { return state_; }
		void setState(UNIT_STATE state) { state_ = state; }

		ProvinceMV* getProvince() { return province_; }
		void setProvince(ProvinceMV* province) {
			province_ = province;
			if (province != nullptr)
				province->getLocation(&location_[0], &location_[1]);
		}

		ProvinceMV* getTarget() { return target_province_; }
		void setTarget(ProvinceMV* province) { target_province_ = province; }

		vector<ProvinceMV*> getPath() {
			return travel_path_;
		}

		void setPath(vector<ProvinceMV*> path) {
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

		void evaluate() {
			setLocation(location_[0] / 1280.0, location_[1] / 720.0);
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
					int home_location[2], target_location[2];
					province_->getLocation(&home_location[0], &home_location[1]);
					target_province_->getLocation(&target_location[0], &target_location[1]);
					location_[0] += (target_location[0] - home_location[0]) * speed_;
					location_[1] += (target_location[1] - home_location[1]) * speed_;
					if (location_[0] == target_location[0] && location_[1] == target_location[1]) { // Add small variance/'magnet' zone for rounding
						province_ = target_province_;
						target_province_ = nullptr; // Get new from path
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
		}
};
