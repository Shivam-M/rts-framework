#pragma once

// class Province;

class Unit {
	private:
		int identifier_ = -1;
		int size_ = 1;
		double skill_factor_ = 1.00;
		Province* location_;

	public:
		Unit(int size, double skill = 1.00, Province* location = nullptr) : size_(size), skill_factor_(skill), location_(location) {}

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		int getSize() { return size_; }
		void setSize(int size) { size_ = size; }

		double getSkill() { return skill_factor_; }
		void setSkill(double skill) { skill_factor_ = skill; }

		Province* getLocation() { return location_; }
		void setLocation(Province* location) { location_ = location; }

		void evaluate() {}
};
