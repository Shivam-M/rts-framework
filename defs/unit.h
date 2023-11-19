#pragma once

class Unit {
	private:
		int identifier_ = -1;
		int size_ = 1;
		double skill_factor_ = 1.00;
		double speed_ = 0.05;
		int location_[2] = { 0, 0 };
		Province* province_ = nullptr;
		Province* target_province_ = nullptr;
		Province** travel_path_ = nullptr;

	public:
		Unit(int identifier, int size, double skill = 1.00, Province* province = nullptr) : identifier_(identifier), size_(size), skill_factor_(skill) { setProvince(province); }

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

		Province* getProvince() { return province_; }
		void setProvince(Province* province) {
			province_ = province;
			if (province != nullptr)
				province->getLocation(&location_[0], &location_[1]);
		}

		Province* getTarget() { return target_province_; }
		void setTarget(Province* province) { target_province_ = province; }

		void evaluate() {
			int home_location[2] = { 0, 0 };
			int target_location[2] = { 0, 0 };
			if (target_province_ != nullptr && province_ != nullptr) {
				province_->getLocation(&home_location[0], &home_location[1]);
				target_province_->getLocation(&target_location[0], &target_location[1]);
				location_[0] += (target_location[0] - home_location[0]) * speed_;
				location_[1] += (target_location[1] - home_location[1]) * speed_;
				if (location_[0] == target_location[0] && location_[1] == target_location[1]) { // Add small variance/'magnet' zone for rounding
					province_ = target_province_;
					target_province_ = nullptr; // Get new from path
				}
			}
		}
};
