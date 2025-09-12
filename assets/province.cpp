#include "province.h"

#include "moveable.h"

using namespace std;

Province::Province(int id) : identifier(id) {}

void Province::initiate_siege(Unit* unit, Colour colour) {
	besieger = unit;
	ColourShift colourshift = ColourShift(get_colour(), colour);
	colourshift.speed = 0.03f;
	colourshift.conditionalise(&SiegeID);
	set_colour_shift(colourshift);
	state = State::Besieging;
}

void Province::progress_siege(float amount) {
	if (state != State::Besieging) return;

	siege_progress += amount;

	if (siege_progress >= 100) {
		controller = besieger;
		besieger = nullptr;
		state = State::Besieged;
		SiegeID++;
	}
}

void Province::end_siege() {
	state = State::Normal;
	siege_progress = 0;
	besieger = nullptr;
	SiegeID++;
}

vector<Province*> Province::find_shortest_path(Province* source, Province* destination) { // TODO: Cache paths
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
