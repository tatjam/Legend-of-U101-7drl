#pragma once
#include <vector>
#include "libtcod.h"
#include "../defines.h"
#include "FlightCrew.h"
#include "../Speech.h"
#include "../flight/FlightMap.h"

#include "workbench/Sonar.h"
#include "workbench/Radio.h"
#include "workbench/Targeting.h"
#include "workbench/Machines.h"
#include "workbench/Maneouver.h"
#include "workbench/Battery.h"
#include "workbench/Periscope.h"

class FlightScene;

struct VehicleTile
{
	// Health always goes from 0-1
	float health;

	float water;
	float prev_water;

	int ch;
	TCODColor fore;
	TCODColor back;

	bool really_blocks_water;
	bool blocks_water;
	bool blocks_player;
	bool blocks_light;

	bool is_outside;
};

struct Torpedo
{
	int x, y;
};

struct TorpedoSlot 
{
	int x, y;
	bool has_torpedo;
	int crew_coming;
};


class Vehicle
{
private:

	VehicleTile dummy_tile;
	VehicleTile& get_tile_for_water(int x, int y);

	void update_water_flow(float dt);
	void update_crew(float dt);
	void update_bubbles(float dt);

	void do_context_menu(TCODConsole* target, int ox, int oy, TCOD_mouse_t pos, int torpedo_hlight);

public:

	FlightScene* scene;

	static constexpr float WATER_FLOW_RATE = 4.0f;
	static constexpr float WATER_PUMP_RATE = 2.0f;
	static constexpr float REPAIR_RATE = 0.25f;

	// Total added water level, having too much water slows the submarine
	// considerably
	float water_level;
	// Water added per second
	float water_flow;

	float noise;

	float water_tick;

	float battery_a;
	float battery_b;
	float battery_aux;
	float diesel;

	bool engines_on_diesel;

	float possible_speed;

	bool torpedo_0;
	bool torpedo_1;
	bool torpedo_2;

	TCODMap* tcod_map;

	FlightCrew* selected;
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

	std::vector<TorpedoSlot> torpedo_slots;

	float flip_timer;

	SoLoud::Wav engines_high;
	SoLoud::Wav engines_low;
	SoLoud::Wav moving_fast;
	SoLoud::Wav moving_slow;
	SoLoud::Wav waterflow;

	SoLoud::Wav shock;

	SoLoud::handle engines_high_h;
	SoLoud::handle engines_low_h;
	SoLoud::handle moving_fast_h;
	SoLoud::handle moving_slow_h;
	SoLoud::handle waterflow_h;

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
	std::vector<FlightCrew> crew;
	std::vector<Workbench*> workbenches;

	Workbench *periscope, *battery;
	Sonar* sonar;
	Radio* radio;
	Maneouver* maneouver;
	Machines* machines;
	Targeting* targeting;

	FlightCrew* get_captain();

	int m0x, m0y, m1x, m1y;

	void update(float dt);
	void draw(TCODConsole* target, int x, int y);
	void draw_window(TCODConsole* target);

	void move_order(Direction dir);
	void speed_order(Speed speed);

	void damage(float power, int x = - 1, int y = - 1);

	// Consumes half the SHK battery
	void electric_shock();

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

