#pragma once
#include <libtcod.hpp>
#include "../defines.h"

struct BuildingTile
{
	bool is_wall;
	bool seen_before;
	bool seen;
	bool seen_by_someone;

	bool blood;
	bool alien_blood;

	BuildingTile()
	{
		is_wall = true;
		seen_before = false;
		seen = false;
		
		blood = 0.0f;
		alien_blood = 0.0f;
	}
};

class BuildingMap;

enum MonsterType
{
	M_ARACHNID,
	M_ANT,
	M_GOLEM,
	M_NEST,
	M_ROBOT,
	M_GRUNT,
	M_ANCIENT,
	M_COMPUTER,

	MONSTER_COUNT
};

struct EmbarkMonster
{
	TCODPath* path;
	float walk_interval;
	
	float walkt;
	int x, y;
	float health;

	float attackt;
	float attack_interval;
	float damage;

	bool seen;

	float t0, t1, t2;

	MonsterType type;

	EmbarkMonster(MonsterType type, TCODMap* map);

	int get_symbol();
	TCODColor get_foreground();

};


class BSPListener : public ITCODBspCallback
{
private:
	BuildingMap* map;
	int room_n;
	int last_x, last_y;
public:
	BSPListener(BuildingMap* map);
	virtual bool visitNode(TCODBsp* node, void* user) override;

};

class BuildingMap
{
public:

	bool organic;

	std::vector<EmbarkMonster> monsters;

	int min_size, max_size;

	int spawn_x0, spawn_y0, spawn_x1, spawn_y1;

	void dig(int x1, int y1, int x2, int y2);
	void create_room(bool first, int x1, int y1, int x2, int y2);

	int width, height;

	TCODMap tcod_map;

	std::vector<BuildingTile> tiles;

	void draw(TCODConsole& target);

	BuildingMap(int w, int h, bool organic = false);
	~BuildingMap();
};

