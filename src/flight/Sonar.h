#pragma once

#include "libtcod.h"
#include "soloud.h"
#include "soloud_wav.h"

#include "FlightMap.h"
#include "../Drawing.h"

class Sonar
{
public:

	SoLoud::Soloud* soloud;

	TCODConsole console;
	float sonar_radius;
	bool sonar_active;
	SoLoud::Wav ping;

	void update(float dt);
	void draw(int rx, int ry);
	void draw_world(int px, int py, FlightMap& map);

	Sonar(SoLoud::Soloud* soloud);
	~Sonar();
};

