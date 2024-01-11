#ifndef PROVINCE_NEW_H
#define PROVINCE_NEW_H
#pragma once

#include "../assets/text_new.h"

class ProvinceNew : public MoveableNew {
	enum TERRAIN { FLAT, MOUNTAIN, FOREST, NUM_TERRAINS };
	enum PROV_STATE { NORMAL, BESIEGING, UNDER_CONTROL };

private:
	int identifier_ = -1;
	int level_ = 0;
	double value_ = 0.50;

	string name_ = "Generic Province";
	string name = "Generic Province";
	Colour2 colour_ = Colour2(255, 255, 255);
	TERRAIN terrain_ = FLAT;
	PROV_STATE state_ = NORMAL;
	vector<ProvinceNew*> neighbours_;

public:
	ProvinceNew(int identifier, string name) : identifier_(identifier), name_(name) {}

	void setID(int identifier) { identifier_ = identifier; }
	int getID() { return identifier_; }

	void incrementLevel(int amount = 1) { level_ += amount; }
	void setLevel(int level) { level_ = level; };
	int getLevel() { return level_; }

	double getValue() { return value_; }
	void setValue(double value) { value_ = value; }

	void setTerrain(TERRAIN terrain) { terrain_ = terrain; }
	TERRAIN getTerrain() { return terrain_; }

	void setState(PROV_STATE state) { state_ = state; }
	PROV_STATE getState() { return state_; }

	vector<ProvinceNew*> getNeighbours() {
		return neighbours_;
	}

	void addNeighbour(ProvinceNew* neighbour) {
		neighbours_.push_back(neighbour);
	}

	bool isNeighbour(ProvinceNew* other) {
		return find(neighbours_.begin(), neighbours_.end(), other) != neighbours_.end();
	}

	void evaluate() {
		if (text != nullptr) {
			text->setLocation(location.x + text_offset.x, location.y + text_offset.y);
			text->setContent(getName() + " [" + to_string(getID()) + "] - (" + to_string(location.x) + ", " + to_string(location.y) + ")");
		}
	}

	bool operator==(const ProvinceNew& other) const {
		return identifier_ == other.identifier_ && name_ == other.name_;
	}
};
#endif