#pragma once
#include <vector>
#include <libtcod.h>

class Status
{
public:

	std::vector<std::string> strings;

	// x1 is ignored, we are always full width
	void draw(int x0, int y0, int x1, int y1);

	Status();
	~Status();
};

