#include "FlightMap.h"




FlightMap::FlightMap(int width, int height, size_t seed) : vmap(width, height)
{
	this->width = width;
	this->height = height;

	tiles.resize(width * height);

	// Cavern style map
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			tiles[y * width + x].wall = true;
		}
	}

	TCODRandom rng = TCODRandom(seed);

	int dx = width / 2, dy = height / 2;
	int open = 0;
	int target_open = rng.getFloat(0.3, 0.5) * width * height;
	int max_steps = 100000;
	int steps = 0;

	while (open < target_open && steps < max_steps)
	{
		if (tiles[dy * width + dx].wall)
		{
			tiles[dy * width + dx].wall = false;
			open++;
		}

		if (rng.getFloat(0.0, 1.0) >= 0.5)
		{
			if (rng.getFloat(0.0, 1.0) >= 0.5)
			{
				dx++;
			}
			else
			{
				dx--;
			}
		}
		else
		{
			if (rng.getFloat(0.0, 1.0) >= 0.5)
			{
				dy++;
			}
			else
			{
				dy--;
			}
		}

		if (dx < 1)
		{
			dx = 1;
		}

		if (dx >= width - 1)
		{
			dx = width - 2;
		}

		if (dy < 1)
		{
			dy = 1;
		}

		if (dy >= height - 1)
		{
			dy = height - 2;
		}



		steps++;
	}

	// TODO: Generate some bases or tunnels

	// Generate the visibility map
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			vmap.setProperties(x, y, !tiles[y * width + x].wall, !tiles[y * width + x].wall);
		}
	}
}

FlightMap::~FlightMap()
{
}
