#include "FlightMap.h"
#include "entity/Buildings.h"
#include "entity/Monsters.h"

static float grad_func(float x)
{
	float v = 1 - sqrt(x);
	return v * v * v * v;
}

MapTile FlightMap::get_subtile(float x, float y)
{


	int tx = (int)floor(x);
	int ty = (int)floor(y);

	float sx = x - (float)tx;
	float sy = y - (float)ty;

	float dec = 20.0f;

	sx = floor(sx * dec) / dec;
	sy = floor(sy * dec) / dec;

	x = (float)tx + sx;
	y = (float)ty + sy;

	MapTile tile = get_tile(tx, ty);
	if (tile != WALL)
	{
		// We add some noise to borders with a wall on them
		bool up = false;
		bool right = false;
		bool left = false;
		bool down = false;

		up = get_tile(tx, ty - 1) == WALL;
		right = get_tile(tx + 1, ty) == WALL;
		left = get_tile(tx - 1, ty) == WALL;
		down = get_tile(tx, ty + 1) == WALL;

		float grad = 0.0f;
		if (up)
		{
			grad += grad_func(sy);
		}
		if (left)
		{
			grad += grad_func(sx);
		}
		if (down)
		{
			grad += grad_func(1 - sy);
		}
		if (right)
		{
			grad += grad_func(1 - sx);
		}

		if (grad >= 1.0f)
		{
			grad = 1.0f;
		}
		
		if (grad <= 0.0f)
		{
			grad = 0.0f;
		}

		float coords[2];
		coords[0] = x * 1.0f;
		coords[1] = y * 1.0f;

		if ((sub_tile_noise.get(&coords[0]) + 1.0f) * 0.5f <= grad)
		{
			return WALL;
		}
		else
		{
			return tile;
		}
	}

	return tile;
}

std::vector<FlightEntity*>& FlightMap::get_entities()
{
	return entities;
}

/*
int FlightMap::get_entity_symbol(EntityType type)
{
	switch (type)
	{
	case E_NEST: return 15;
	case E_STATION: return 234;
	case E_BASE: return 127;
	case E_CRAWLER: return 157;
	case E_WORM: return 126;
	case E_GUARDIAN: return 236;
	case E_SERPENT: return 21;
	case E_BOMB: return 155;
	case E_BITER: return 224;
	case E_SUBMARINE: return '!';
	default: return '?';
	}
}
*/

template<typename T, size_t NUM>
void create_entities(std::vector<FlightEntity*>* target, FlightMap* map, FlightScene* scene)
{
	int n = 0;
	while (n < NUM)
	{
		int x = g_random->getInt(0, map->width - 1);
		int y = g_random->getInt(0, map->height - 1);

		if (map->tiles[y * map->width + x] != WALL)
		{
			T* crawler = new T();
			crawler->set_position((float)x + 0.5f, (float)y + 0.5f);
			crawler->init(map, scene);
			target->push_back(crawler);
			n++;
		}
	}
}

FlightMap::FlightMap(int width, int height, size_t seed, FlightScene* scene) : vmap(width, height), sub_tile_noise(2)
{
	this->width = width;
	this->height = height;

	tiles.resize(width * height);

	// Cavern style map
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			tiles[y * width + x] = WALL;
		}
	}


	int dx = width / 2, dy = height / 2;
	int open = 0;
	int target_open = g_random->getFloat(0.3, 0.5) * width * height;
	int max_steps = 100000;
	int steps = 0;

	while (open < target_open && steps < max_steps)
	{
		if (tiles[dy * width + dx] == WALL)
		{
			tiles[dy * width + dx] = CLEAR;
			open++;
		}

		if (g_random->getFloat(0.0, 1.0) >= 0.5)
		{
			if (g_random->getFloat(0.0, 1.0) >= 0.5)
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
			if (g_random->getFloat(0.0, 1.0) >= 0.5)
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
			vmap.setProperties(x, y, !(tiles[y * width + x] == WALL), !(tiles[y * width + x] == WALL));
		}
	}

	TCODNoise noise = TCODNoise(2, g_random, TCOD_NOISE_PERLIN);

	// Generate air map
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			float coords[2];
			coords[0] = (float)x / 10.0f;
			coords[1] = (float)y / 10.0f;

			if (noise.get(&coords[0]) >= 0.0f && tiles[y * width + x] == CLEAR)
			{
				tiles[y * width + x] = AIR;
			}
		}
	}

	int stations = 0;

	while (stations < 3)
	{
		// Place stations far away from the center
		int x = g_random->getInt(0, width - 1);
		int y = g_random->getInt(0, height - 1);

		float xf = (float)(x - width / 2) / (float)(width / 2);
		float yf = (float)(y - height / 2) / (float)(height / 2);

		float dist = sqrt(xf * xf + yf * yf);

		if (dist >= 0.7f && tiles[y * width + x] != WALL)
		{
			tiles[y * width + x] = STATION;
			stations++;
			EntityStation* n = new EntityStation();
			n->set_position((float)x + 0.5f, (float)y + 0.5f);
			n->init(this, scene);
			entities.push_back(n);
		}
		
	}

	int nests = 0;

	// Place nests semi randomly, with a bias from far away from the center
	while (nests < 15)
	{
		// Place stations far away from the center
		int x = g_random->getInt(0, width - 1);
		int y = g_random->getInt(0, height - 1);

		if (tiles[y * width + x] != WALL)
		{
			tiles[y * width + x] = NEST;
			nests++;
			EntityNest* n = new EntityNest();
			n->set_position((float)x + 0.5f, (float)y + 0.5f);
			n->init(this, scene);
			entities.push_back(n);
		}

	}

	// Place allied bases
	int bases = 1;
	while (bases < 5)
	{
		// Place stations far away from the center
		int x = g_random->getInt(0, width - 1);
		int y = g_random->getInt(0, height - 1);

		if (tiles[y * width + x] != WALL)
		{
			tiles[y * width + x] = BASE;
			bases++;
			lbases.push_back(std::make_pair(x, y));
			EntityBase* n = new EntityBase();
			n->set_position((float)x + 0.5f, (float)y + 0.5f);
			n->init(this, scene);
			entities.push_back(n);
		}

	}


	tiles[(height / 2) * height + (width / 2)] = BASE;
	lbases.push_back(std::make_pair(width / 2, height / 2));
	EntityBase* n = new EntityBase();
	n->set_position((float)(width / 2) + 0.5f, (float)(height / 2) + 0.5f);
	n->init(this, scene);
	entities.push_back(n);

	// Spawn some entities across the map
	if (width > 20)
	{
		create_entities<EntityCrawler, 10>(&entities, this, scene);
		create_entities<EntityWorm, 10>(&entities, this, scene);
		create_entities<EntitySerpent, 4>(&entities, this, scene);
	}


	for (FlightEntity* ent : entities) 
	{
		if (ent->get_type() == E_NEST)
		{
			// Spawn a guardian near it
			int guardians = g_random->getInt(-1, 2);
			int g = 0;
			while (g < guardians)
			{
				// Place stations far away from the center
				float ox = g_random->getFloat(-3.0f, 3.0f);
				float oy = g_random->getInt(-3.0f, 3.0f);

				ox += ent->get_x();
				oy += ent->get_y();

				if (get_tile((int)floor(ox), (int)floor(oy)) != WALL)
				{
					EntityGuardian* ent = new EntityGuardian();
					ent->init(this, scene);
					ent->set_position(ox, oy);
					entities.push_back(ent);

					g++;
				}

			}
		}
	}

	/*
	REMEMBER TO ADD SOUND!!!
	EntityBomb* ent = new EntityBomb();
	ent->init(this, scene);
	ent->set_position(51.0f, 50.5f);
	entities.push_back(ent);*/

	/*EntitySerpent* ent = new EntitySerpent();
	ent->init(this, scene);
	ent->set_position(51.0f, 50.5f);
	entities.push_back(ent);
	*/

}

FlightMap::~FlightMap()
{
}
