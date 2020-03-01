#pragma once
#include <libtcod.h>
#include "../defines.h"
#include "../Status.h"
#include "FlightMap.h"
#include "soloud.h"
#include "soloud_wav.h"
#include "Sonar.h"
#include "../Drawing.h"
#include "../Help.h"


constexpr int map_size = 512;

enum Window
{
	NONE,
	SONAR
};

class FlightScene
{
public:

	SoLoud::Soloud* soloud;

	SoLoud::Wav underwater;

	FlightMap map;

	Window open_window;


	Sonar sonar;
	
	bool help_open;
	std::string help_str;

	int px, py;


	TCODConsole spaceship;
	Status* status;

	void turn(TCOD_key_t key);
	void update(float dt);
	void render();

	FlightScene(Status* status);
	~FlightScene();
};

