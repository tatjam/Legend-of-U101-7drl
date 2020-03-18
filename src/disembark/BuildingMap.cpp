#include "BuildingMap.h"
#include "../flight/Gamemaster.h"


void BuildingMap::dig(int x1, int y1, int x2, int y2)
{
	if (x2 < x1)
	{
		std::swap(x2, x1);
	}

	if (y2 < y1)
	{
		std::swap(y2, y1);
	}

	for (int x = x1; x <= x2; x++)
	{
		for (int y = y1; y <= y2; y++)
		{
			if (x > 0 && y > 0 && x < width - 1 && y < height - 1)
			{
				tiles[y * width + x].is_wall = false;
			}
		}
	}
}

void BuildingMap::create_room(bool first, int x1, int y1, int x2, int y2)
{
	dig(x1, y1, x2, y2);

	if (first)
	{
		// Set spawn location
		spawn_x0 = x1;
		spawn_y0 = y1;
		spawn_x1 = x2;
		spawn_y1 = y2;
	}
	else
	{
		// Spawn stuff in the room
	}
}

void BuildingMap::draw(TCODConsole& target)
{
	int ch = '#';
	if (!organic)
	{
		ch = 240;
	}

	TCODColor fore_b = TCODColor(200, 200, 200);
	TCODColor fore_s = TCODColor(70, 70, 70);
	if (!organic)
	{
		fore_b = TCODColor(180, 180, 255);
		fore_s = TCODColor(60, 60, 100);
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			TCODColor fore;
			TCODColor back;

			if (tiles[y * width + x].seen)
			{
				fore = fore_b;
				back = TCODColor(20, 20, 20);

				if (tiles[y * width + x].alien_blood)
				{
					back = TCODColor(54, 0, 217);
				}
				else if (tiles[y * width + x].blood)
				{
					back = TCODColor(102, 0, 26);
				}
			}
			else if (tiles[y * width + x].seen_before)
			{
				fore = fore_s;
				back = TCODColor(0, 0, 0);
			}
			else
			{
				fore = TCODColor::black;
				back = TCODColor::black;
			}


			if (tiles[y * width + x].is_wall)
			{
				target.setChar(x, y, ch);
			}
			else
			{
				fore.setValue(fore.getValue() * 0.5f);
				target.setChar(x, y, '.');
			}

			target.setCharForeground(x, y, fore);
			target.setCharBackground(x, y, back);
		}
	}

	for (int i = 0; i < monsters.size(); i++)
	{
		if (tiles[monsters[i].y * width + monsters[i].x].seen)
		{
			target.setChar(monsters[i].x, monsters[i].y, monsters[i].health > 0.0f ?  monsters[i].get_symbol() : '*');
			target.setCharForeground(monsters[i].x, monsters[i].y, monsters[i].get_foreground());
		}
	}


}

BuildingMap::BuildingMap(int w, int h, bool organic) : tcod_map(w, h)
{
	this->organic = organic;

	width = w;
	height = h;
	tiles.resize(w * h);


	// Generate the map
	TCODBsp root = TCODBsp(0, 0, w, h);

	min_size = g_random->getInt(4, 8);
	max_size = g_random->getInt(min_size, min_size + 8);

	root.splitRecursive(g_random, 8, max_size, max_size,
		g_random->getFloat(1.3f, 1.8f), g_random->getFloat(1.3f, 1.8f));

	BSPListener listener = BSPListener(this);
	root.traverseInvertedLevelOrder(&listener, nullptr);
	
	if (organic)
	{
		// Do some random digging
		int it = 0;
		int to_dig = (int)(g_random->getFloat(0.2f, 0.5f) * width * height);

		int dx = g_random->getInt(0, width);
		int dy = g_random->getInt(0, height);

		while (it < to_dig)
		{
			dx += g_random->getInt(-1, 1);
			dy += g_random->getInt(-1, 1);

			if (dx > 0 && dy > 0 && dx < width - 1 && dy < height - 1)
			{
				tiles[dy * width + dx].is_wall = false;
				it++;
			}
			else
			{
				dx = g_random->getInt(0, width);
				dy = g_random->getInt(0, height);
			}


		}
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			tcod_map.setProperties(x, y, !tiles[y * width + x].is_wall, !tiles[y * width + x].is_wall);
		}
	}

	// Spawn bois
	int wanted[MONSTER_COUNT] = { 0 };
	int placed[MONSTER_COUNT] = { 0 };

	if (organic)
	{
		wanted[M_ARACHNID] = g_random->getInt(6, 14);
		wanted[M_ANT] = g_random->getInt(4, 9);
		wanted[M_GOLEM] = g_random->getInt(2, 5);
		wanted[M_NEST] = 1;
	}
	else
	{
		wanted[M_ARACHNID] = g_random->getInt(0, 5);
		wanted[M_ANT] = g_random->getInt(0, 5);
		wanted[M_GOLEM] = g_random->getInt(0, 2);
		wanted[M_ROBOT] = g_random->getInt(3, 10);
		wanted[M_GRUNT] = g_random->getInt(1, 5);
	}

	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		while (placed[i] < wanted[i])
		{
			int x = g_random->getInt(0, width - 1);
			int y = g_random->getInt(0, height - 1);
			if (x >= spawn_x0 && y >= spawn_y0 && x < spawn_x1 && y < spawn_y1) 
			{
				continue;
			}

			if (tiles[y * width + x].is_wall)
			{
				continue;
			}

			placed[i]++;
			EmbarkMonster m = EmbarkMonster((MonsterType)i, &tcod_map);
			m.x = x;
			m.y = y;
		
			monsters.push_back(m);
		}
	}

	

}

BuildingMap::~BuildingMap()
{
}

BSPListener::BSPListener(BuildingMap* map)
{
	this->map = map;
	room_n = 0;
}

bool BSPListener::visitNode(TCODBsp * node, void * user)
{
	if (node->isLeaf())
	{
		int x, y, w, h;
		w = g_random->getInt(map->min_size, node->w - 2);
		h = g_random->getInt(map->min_size, node->h - 2);
		x = g_random->getInt(node->x + 1, node->x + node->w - w - 1);
		y = g_random->getInt(node->y + 1, node->y + node->h - h - 1);
		map->create_room(room_n == 0, x, y, x + w - 1, y + h - 1);

		if (room_n != 0)
		{
			map->dig(last_x, last_y, x + w / 2, last_y);
			map->dig(x + w / 2, last_y, x + w / 2, y + h / 2);
		}

		last_x = x + w / 2;
		last_y = y + h / 2;
		room_n++;
	}

	return true;
}
