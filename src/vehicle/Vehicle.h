#pragma once
#include <vector>
#include "libtcod.h"
#include "../defines.h"
#include "Crewmember.h"
#include "../Speech.h"
#include "../flight/FlightMap.h"

#include "workbench/Sonar.h"
#include "workbench/Radio.h"
#include "workbench/Listening.h"
#include "workbench/Machines.h"
#include "workbench/Maneouver.h"
#include "workbench/Battery.h"
#include "workbench/Periscope.h"


struct VehicleTile
{
	float health;

	float water;
	int ch;
	TCODColor fore;
	TCODColor back;

	bool blocks_water;
	bool blocks_player;
	bool blocks_light;

	bool is_outside;
};

struct Torpedo
{
	int x, y;
};



class Vehicle
{
public:

	TCODMap* tcod_map;

	Crewmember* selected;
	bool in_context_menu;
	int ctx_x, ctx_y;

	bool breathing;

	float blinkt;
	bool blink;

	struct Bubble
	{
		float x, y;
		int ch;
		float b;
		float t;
		float f;

		Bubble()
		{
			t = 0.0f;
			b = 0.0f;
			f = g_random->getFloat(0.4f, 2.0f);

			if (g_random->getFloat(0.0f, 1.0f) >= 0.5f)
			{
				ch = 'o';
			}
			else
			{
				ch = 'O';
			}
		}
	};

	std::vector<Bubble> bubbles;

	float flip_timer;

	SoLoud::Wav engines_high;
	SoLoud::Wav engines_low;
	SoLoud::Wav moving_fast;
	SoLoud::Wav moving_slow;

	SoLoud::handle engines_high_h;
	SoLoud::handle engines_low_h;
	SoLoud::handle moving_fast_h;
	SoLoud::handle moving_slow_h;

	Workbench* workbench_open;

	// World tile position, floor to get int position
	float x, y;

	float angle;
	float velocity;

	int width, height;

	// Tiles are the "dumb" foundation of the submarine,
	// they only have visual and physical (water) effects
	std::vector<VehicleTile> tiles;
	std::vector<Torpedo> torpedoes;
	std::vector<Crewmember> crew;
	std::vector<Workbench*> workbenches;

	Workbench *periscope, *machines, *battery, *listening;
	Sonar* sonar;
	Radio* radio;
	Maneouver* maneouver;

	int m0x, m0y, m1x, m1y;

	void update(float dt);
	void draw(TCODConsole* target, int x, int y);
	void draw_window(TCODConsole* target);

	void move_order(Direction dir);
	void speed_order(Speed speed);

	std::pair<int, int> get_tile()
	{
		return std::make_pair((int)floor(x), (int)floor(y));
	}

	std::pair<float, float> get_subtile()
	{
		auto t = get_tile();
		return std::make_pair(x - t.first, y - t.second);
	}

	FlightMap* in_map;

	Vehicle(FlightMap* map);
	~Vehicle();
};

