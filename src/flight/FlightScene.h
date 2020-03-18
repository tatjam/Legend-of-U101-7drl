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

constexpr int map_size = 100;

struct ExplosionEffect
{
	float x, y, t, dist;
};

class FlightScene
{
public:

	FlightEntity* embark_target;
	FlightEntity* possible_embark;

	float t = 0.0f;
	bool sent_start_message = false;

	
	SoLoud::Soloud* soloud;

	SoLoud::Wav underwater;
	SoLoud::Wav explo_near_dist;
	SoLoud::Wav explo_far_dist;
	SoLoud::Wav explo_med_dist;
	SoLoud::Wav explo_hit;

	FlightMap map;

	Vehicle vehicle;

	bool help_open;

	std::vector<ExplosionEffect> expl_effects;

	Status* status;

	void update(float dt);
	void render();

	void explosion(float x, float y, float power);

	void shut_up();

	FlightScene();
	~FlightScene();
};

