#pragma once
#include <unordered_set>
#include <queue>
#include "../assets/text.h"

class Province : public Moveable {
	enum TERRAIN { FLAT, MOUNTAIN, FOREST, NUM_TERRAINS };
	enum PROV_STATE { NORMAL, BESIEGING, UNDER_CONTROL };

	private:
		int identifier_ = -1;
		int level_ = 0;
		float value_ = 0.50;

		TERRAIN terrain_ = FLAT;
		PROV_STATE state_ = NORMAL;
		vector<Province*> neighbours_;
		vector<Unit*> stationed_units;
		Nation* nation_ = nullptr;

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

		void evaluate() {
			return;
			if (text != nullptr) {
				text->setLocation(location.x + text_offset.x, location.y + text_offset.y);
				text->setContent(getName() + " [" + to_string(getID()) + "] - (" + to_string(location.x) + ", " + to_string(location.y) + ")");
			}
		}

		static vector<Province*> getShortestPath(Province* source, Province* destination) { // replace?
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
