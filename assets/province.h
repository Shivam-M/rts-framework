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

static int SiegeID = 0;

class Province : public Moveable {
	enum class Terrain { Flat, Mountain, Forest, _Count };
	enum class State { Normal, Besieging, Besieged };

	const string TERRAIN_NAMES[static_cast<int>(Terrain::_Count)] = { "Plains", "Mountains", "Forest" };

	private:
		Terrain terrain_ = Terrain::Flat;
		vector<Province*> neighbours_;
		vector<Unit*> units_;

	public:
		int identifier = -1;
		float value = 0.50f;
		float siege_progress = 0.f;
		Nation* nation = nullptr;
		Unit* besieger = nullptr;
		Unit* controller = nullptr;
		State state = State::Normal;

		Province(int id);

		void evaluate() {}
		void initiate_siege(Unit* unit, Colour colour);
		void progress_siege(float amount);
		void end_siege();

		const string& get_terrain_name() const { return TERRAIN_NAMES[static_cast<int>(terrain_)]; }

		const vector<Province*>& get_neighbours() { return neighbours_; }
		void register_neighbour(Province* neighbour) { neighbours_.push_back(neighbour); }
		bool is_neighbour(Province* other) { return find(neighbours_.begin(), neighbours_.end(), other) != neighbours_.end(); }

		const vector<Unit*>& get_stationed_units() { return units_; }
		void register_unit(Unit* unit) { units_.push_back(unit); }
		void deregister_unit(Unit* unit) { units_.erase(remove(units_.begin(), units_.end(), unit), units_.end()); }

		static vector<Province*> find_shortest_path(Province* source, Province* destination);

		bool operator==(const Province& other) const { return identifier == other.identifier; }
};
