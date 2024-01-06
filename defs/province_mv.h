#ifndef PROVINCE_H
#define PROVINCE_H
#pragma once

class ProvinceMV: public Moveable {
	enum TERRAIN { FLAT, MOUNTAIN, FOREST, NUM_TERRAINS };
	enum PROV_STATE { NORMAL, BESIEGING, UNDER_CONTROL };

private:
	int identifier_ = -1;
	int level_ = 0;
	int location_[2] = { -1, -1 };
	double value_ = 0.50;

	string name_ = "Generic Province";
	string name = "Generic Province";
	Colour2 colour_ = Colour2(255, 255, 255);
	TERRAIN terrain_ = FLAT;
	PROV_STATE state_ = NORMAL;
	vector<ProvinceMV*> neighbours_;

	public:
	ProvinceMV(int identifier, string name) : identifier_(identifier), name_(name) {}

	void setID(int identifier) { identifier_ = identifier; }
	int getID() { return identifier_; }

	void incrementLevel(int amount = 1) { level_ += amount; }
	void setLevel(int level) { level_ = level; };
	int getLevel() { return level_; }

	void setLocation(double x, double y) { location[0] = x, location[1] = y; location_[0] = x * 1280.0; location_[1] =  y * 720.0;
	if (text != nullptr) {
		setBounds(0, -0.0125);
		//temp
		evaluate();
	}
	}

	void getLocation(int* x, int* y) { *x = location_[0], * y = location_[1]; }
	void setGridLocation(int x, int y) { 
		location_[0] = x, location_[1] = y; 
		setLocation(x / 1280.0, y / 720.0);
	}

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

	vector<ProvinceMV*> getNeighbours() {
		return neighbours_;
	}

	void addNeighbour(ProvinceMV* neighbour) {
		neighbours_.push_back(neighbour);
	}

	bool isNeighbour(ProvinceMV* other) {
		 return find(neighbours_.begin(), neighbours_.end(), other) != neighbours_.end();
	}

	void evaluate() {
		if (text != nullptr) {
			text->setLocation(Moveable::getLocation()[0] + bounds[0], Moveable::getLocation()[1] + bounds[1]);
			text->setContent(getName() + " [" + to_string(getID()) + "] - (" + to_string(location_[0]) + ", " + to_string(location_[1]) + ")");
		}
	}

	bool operator==(const ProvinceMV& other) const {
		return identifier_ == other.identifier_ && name_ == other.name_;
	}
};
#endif