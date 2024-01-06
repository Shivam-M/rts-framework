#pragma once

class Province {
	enum TERRAIN { FLAT, MOUNTAIN, FOREST, NUM_TERRAINS };
	enum PROV_STATE { NORMAL, BESIEGING, UNDER_CONTROL };

	private:
		int identifier_ = -1;
		int level_ = 0;
		int location_[2] = { -1, -1 };
		double value_ = 0.50;

		string name_ = "Generic Province";
		Colour2 colour_ = Colour2(255, 255, 255);
		TERRAIN terrain_ = FLAT;
		PROV_STATE state_ = NORMAL;

	public:
		Province(int identifier, string name) : identifier_(identifier), name_(name) {}

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		void incrementLevel(int amount = 1) { level_ += amount; }
		void setLevel(int level) { level_ = level; };
		int getLevel() { return level_; }

		void getLocation(int* x, int* y) { *x = location_[0], *y = location_[1]; }
		void setLocation(int x, int y) { location_[0] = x, location_[1] = y; }

		double getValue() { return value_; }
		void setValue(double value) { value_ = value; }

		string getName() { return name_; }
		void setName(string name) { name_ = name; }

		Colour2 getColour() { return colour_; }
		void setColour(Colour2 colour) { colour_ = colour; }

		void setTerrain(TERRAIN terrain) { terrain_ = terrain; }
		TERRAIN getTerrain() { return terrain_; }

		void setState(PROV_STATE state) { state_ = state; }
		PROV_STATE getState() { return state_; }

		void evaluate() {}

		bool operator==(const Province& other) const {
			return identifier_ == other.identifier_ && name_ == other.name_;
		}
};
