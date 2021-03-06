#pragma once
#include "../vehicle/Vehicle.h"
#include "../Popup.h"

// The gamemaster spawns enemies and generates orders from high command, driving the lore forward
// Lore:
// U101 is named after the succesful U101 from WW2, but this time its deployed in the 
// Jupiter Icy Moon, Europa.
// Your task is to get rid of the local population for the building of further colonies.
// BUT you quickly find that you are not the only humans on the icy moon, finding a few
// derelict space stations that you should explore
// The ultimate goal is to find a survivor in the wrecks and find intelligent life outside
// the earth with very similar qualities.
// High command will show opposition to this, eventually turning you against the other
// submarines


class FlightScene;
class EmbarkScene;

class Gamemaster
{
private:

public:

	bool gameover = false;
	bool gamewin = false;

	std::vector<Crewmember*> crew;

	bool embarked;

	FlightScene* flight_scene;
	EmbarkScene* embark_scene;

	bool seen_robot;
	bool seen_grunt;
	
	// The ancient is never seen

	// Activates once we discover intelligent life
	bool is_fatherland_enemy = false;
	int clear_station_count = 0;
	int clear_nest_count = 0;

	void update(float dt);
	void init();

	Gamemaster();
	~Gamemaster();
};

