#pragma once
#include <unordered_set>
#include <queue>
#include "../assets/text.h"

enum PROV_STATE { NORMAL, BESIEGING, UNDER_CONTROL };

static int SiegeID = 0;

class Province : public Moveable {
	enum TERRAIN { FLAT, MOUNTAIN, FOREST, NUM_TERRAINS };
	string TERRAIN_NAMES[3] = {"Plains", "Mountains", "Forest"};

	private:
		int identifier_ = -1;
		int level_ = 0;
		float value_ = 0.50f;
		float siege_progress_ = 0.f;

		TERRAIN terrain_ = FLAT;
		PROV_STATE state_ = PROV_STATE::NORMAL;
		vector<Province*> neighbours_;
		vector<Unit*> stationed_units;
		Nation* nation_ = nullptr;
		Unit* besieger_ = nullptr;
		Unit* controlled_by = nullptr;

	public:
		Province(int identifier) : identifier_(identifier) {}

		void setID(int identifier) { identifier_ = identifier; }
		int getID() { return identifier_; }

		void incrementLevel(int amount = 1) { level_ += amount; }
		void setLevel(int level) { level_ = level; };
		int getLevel() { return level_; }

		float getValue() { return value_; }
		void setValue(float value) { value_ = value; }

		void setTerrain(TERRAIN terrain) { terrain_ = terrain; }
		TERRAIN getTerrain() { return terrain_; }
		string getTerrainName() { return TERRAIN_NAMES[terrain_]; }

		void setState(PROV_STATE state) { state_ = state; }
		PROV_STATE getState() { return state_; }

		vector<Province*> getNeighbours() { return neighbours_; }
		void addNeighbour(Province* neighbour) { neighbours_.push_back(neighbour); }
		bool isNeighbour(Province* other) { return find(neighbours_.begin(), neighbours_.end(), other) != neighbours_.end(); }

		void registerUnit(Unit* unit) { stationed_units.push_back(unit); }
		void deregisterUnit(Unit* unit) { stationed_units.erase(remove(stationed_units.begin(), stationed_units.end(), unit), stationed_units.end()); }
		vector<Unit*> getStationedUnits() { return stationed_units; }

		void setNation(Nation* nation) { nation_ = nation; }
		Nation* getNation() { return nation_; }
		Unit* getBesieger() { return besieger_; }
		Unit* getController() { return controlled_by; } // Should be nation instead as this unit might be dead

		void beginSiege(Unit* unit, Colour colour) {
			besieger_ = unit;
			ColourShift colourshift = ColourShift(getColour(), colour);
			colourshift.speed = 0.03f;
			colourshift.setCondition(&SiegeID);

			/*
			ColourShift2 colourshift2 = ColourShift2(getColour(), colour, 0.03f);
			colourshift2.setCondition(&SiegeID);

			setColourShift2(colourshift2);
			*/

			setColourShift(colourshift);
			setState(BESIEGING);
		}

		void progressSiege(float amount) {
			if (getState() != PROV_STATE::BESIEGING) return;
			if (siege_progress_ >= 100) {
				controlled_by = besieger_;
				besieger_ = nullptr;
				setState(UNDER_CONTROL);
				SiegeID++;
			} else {
				siege_progress_ += amount;
			}
			
		}

		float getSiegeProgress() { return siege_progress_; }

		void endSiege() {
			setState(PROV_STATE::NORMAL);
			siege_progress_ = 0;
			besieger_ = nullptr;
			SiegeID++;
		}

		void evaluate() {
			return;
			if (text != nullptr) {
				text->setLocation(location.x + text_offset.x, location.y + text_offset.y);
				text->setContent(getName() + " [" + to_string(getID()) + "] - (" + to_string(location.x) + ", " + to_string(location.y) + ")");
			}
		}

		static vector<Province*> getShortestPath(Province* source, Province* destination) { // Cache paths
			queue<Province*> q;
			unordered_set<Province*> visited;
			unordered_map<Province*, Province*> previous;

			q.push(source);
			visited.insert(source);
			while (!q.empty()) {
				Province* current = q.front();
				q.pop();

				if (current == destination) {
					vector<Province*> path;
					Province* current_province = destination;
					while (current_province != nullptr) {
						path.insert(path.begin(), current_province);
						current_province = previous[current_province];
					} return path;
				}

				for (Province* neighbor : current->getNeighbours()) {
					if (visited.find(neighbor) == visited.end()) {
						visited.insert(neighbor);
						previous[neighbor] = current;
						q.push(neighbor);
					}
				}
			}
			return vector<Province*>();
		}

		bool operator==(const Province& other) const { return identifier_ == other.identifier_; }

};
