#pragma once
#include <libtcod.h>
#include "../defines.h"
#include "../Status.h"
#include "FlightMap.h"
#include "soloud.h"
#include "soloud_wav.h"

#include "../Drawing.h"
#include "../Help.h"
#include "../vehicle/Vehicle.h"

constexpr int map_size = 500;


class FlightScene
{
public:

	float t = 0.0f;
	bool sent_start_message = false;

	
	SoLoud::Soloud* soloud;

	SoLoud::Wav underwater;

	FlightMap map;

	Vehicle vehicle;

	bool help_open;
	std::string help_str;


	Status* status;

	void update(float dt);
	void render();


	FlightScene();
	~FlightScene();
};

