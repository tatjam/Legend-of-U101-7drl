#pragma once
#include "libtcod.h"
#include "soloud.h"
#include "soloud_speech.h"
#include "soloud_wav.h"
#include "../defines.h"
#include "../Crewmember.h"

#include <string>
#include <array>

class Workbench;


class FlightCrew
{
public:

	Crewmember* gc;

	int x, y;

	// Where we go when called to return to positions
	int cx, cy;

	// The first one is priority (index 0)
	std::vector<Workbench*> assigned;



	FlightCrew(Crewmember* crew);

	bool can_work_in(Workbench* bench);

	void path_to(int dx, int dy, TCODMap& map);

	TCODPath* path;
	float path_t = 0.0f;

	bool has_torpedo = false;
	bool wants_torpedo = false;

	int repair_x = -1, repair_y = -1;

	bool is_repairing = false;
	bool is_pumping = false;

	bool repair_order;
};