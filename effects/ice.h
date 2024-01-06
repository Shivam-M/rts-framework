#ifndef ICE_H
#define ICE_H
#pragma once
#include "../shapes/square.h"
#include <vector>

class Ice : public Moveable {
private:
	int count = 0;

public:
	std::vector<Moveable*>* objects;

	Ice();
	void stop();
	void update(double = 1.0);
};

#endif
