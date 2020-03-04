#pragma once
#include <vector>
#include <libtcod.h>

enum MapTile
{
	CLEAR,
	WALL,
	AIR,
	STATION,
	NEST,
	BASE //< Fatherland base, not derelict station
};

enum EntityType
{
	E_NEST,
	E_STATION,
	E_BASE
};

struct Entity 
{
	EntityType type;
	float x, y;

	Entity(EntityType type, float x, float y)
	{
		this->type = type;
		this->x = x;
		this->y = y;
	}
};

class FlightMap
{
public:

	std::vector<Entity> entities;

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

	std::vector<Entity>& get_entities();

	FlightMap(int width, int height, size_t seed);
	~FlightMap();
};

