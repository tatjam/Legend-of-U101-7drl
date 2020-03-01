#pragma once
#include <vector>
#include <libtcod.h>

struct MapTile
{
	bool wall;
};

class FlightMap
{
public:

	int width, height;
	std::vector<MapTile> tiles;

	TCODMap vmap;

	FlightMap(int width, int height, size_t seed);
	~FlightMap();
};

