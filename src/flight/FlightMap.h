#pragma once
#include <vector>
#include <libtcod.h>
#include "entity/FlightEntity.h"

class FlightScene;

enum MapTile
{
	CLEAR,
	WALL,
	AIR,
	STATION,
	NEST,
	BASE //< Fatherland base, not derelict station
};


class FlightMap
{
public:

	std::vector<FlightEntity*> entities;

	TCODNoise sub_tile_noise;

	// These features are always centered
	std::vector<std::pair<int, int>> lbases;

	int width, height;
	std::vector<MapTile> tiles;

	TCODMap vmap;

	MapTile get_tile(int x, int y)
	{
		if (x < 0 || y < 0 || x >= width || y >= height)
		{
			return WALL;
		}
		else
		{
			return tiles[y * width + x];
		}
	}

	MapTile get_subtile(float x, float y);

	std::vector<FlightEntity*>& get_entities();

	FlightMap(int width, int height, size_t seed, FlightScene* scene);
	~FlightMap();
};

