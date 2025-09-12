#pragma once

#include "moveable.h"

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>

using namespace std;

class Text;
class Nation;
class Unit;

enum PROV_STATE { NORMAL, BESIEGING, UNDER_CONTROL };

static int SiegeID = 0;

class Province : public Moveable {  // TODO: Move definitions to a .cpp file
	enum TERRAIN { FLAT, MOUNTAIN, FOREST, NUM_TERRAINS };
	string TERRAIN_NAMES[NUM_TERRAINS] = {"Plains", "Mountains", "Forest"};

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

		void evaluate() {}

		void set_id(int identifier) { identifier_ = identifier; }
		int get_id() { return identifier_; }

		void increment_level(int amount = 1) { level_ += amount; }
		void set_level(int level) { level_ = level; };
		int get_level() { return level_; }

		float get_value() { return value_; }
		void set_value(float value) { value_ = value; }

		void set_terrain(TERRAIN terrain) { terrain_ = terrain; }
		TERRAIN get_terrain() { return terrain_; }
		const string& get_terrain_name() { return TERRAIN_NAMES[terrain_]; }

		void set_state(PROV_STATE state) { state_ = state; }
		PROV_STATE get_state() { return state_; }

		vector<Province*>& get_neighbours() { return neighbours_; }
		void register_neighbour(Province* neighbour) { neighbours_.push_back(neighbour); }
		bool is_neighbour(Province* other) { return find(neighbours_.begin(), neighbours_.end(), other) != neighbours_.end(); }

		void register_unit(Unit* unit) { stationed_units.push_back(unit); }
		void deregister_unit(Unit* unit) { stationed_units.erase(remove(stationed_units.begin(), stationed_units.end(), unit), stationed_units.end()); }
		vector<Unit*>& get_stationed_units() { return stationed_units; }

		void set_nation(Nation* nation) { nation_ = nation; }
		Nation* get_nation() { return nation_; }

		void set_besieger(Unit* unit) { besieger_ = unit; }

		Unit* get_besieger() { return besieger_; }
		Unit* get_controller() { return controlled_by; } // Should be nation instead as this unit might be dead

		void initiate_siege(Unit* unit, Colour colour) {
			besieger_ = unit;
			ColourShift colourshift = ColourShift(get_colour(), colour);
			colourshift.speed = 0.03f;
			colourshift.conditionalise(&SiegeID);
			set_colour_shift(colourshift);
			set_state(BESIEGING);
		}

		void progress_siege(float amount) {
			if (get_state() != PROV_STATE::BESIEGING) return;

			siege_progress_ += amount;

			if (siege_progress_ >= 100) {
				controlled_by = besieger_;
				besieger_ = nullptr;
				set_state(UNDER_CONTROL);
				SiegeID++;
			}
		}

		float get_siege_progress() { return siege_progress_; }

		void end_siege() {
			set_state(PROV_STATE::NORMAL);
			siege_progress_ = 0;
			besieger_ = nullptr;
			SiegeID++;
		}

		static vector<Province*> find_shortest_path(Province* source, Province* destination) { // TODO: Cache paths
			queue<Province*> q;
			unordered_set<Province*> visited;
			unordered_map<Province*, Province*> previous;
			vector<Province*> path;

			q.push(source);
			visited.insert(source);
			while (!q.empty()) {
				Province* current = q.front();
				q.pop();

				if (current == destination) {
					Province* current_province = destination;
					while (current_province != nullptr) {
						path.insert(path.begin(), current_province);
						current_province = previous[current_province];
					} return path;
				}

				for (Province* neighbor : current->get_neighbours()) {
					if (visited.find(neighbor) == visited.end()) {
						visited.insert(neighbor);
						previous[neighbor] = current;
						q.push(neighbor);
					}
				}
			}
			return path;
		}

		bool operator==(const Province& other) const { return identifier_ == other.identifier_; }

};
