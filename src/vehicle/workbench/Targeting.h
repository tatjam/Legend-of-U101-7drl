#pragma once

#include <string>
#include <vector>
#include "../../Date.h"
#include "libtcod.h"

#include "Workbench.h"

class FlightEntity;

class Targeting : public Workbench
{
public:

	FlightEntity* identify;
	float identify_timer;

	constexpr static float IDENTIFY_TIME = 10.0f;

	FlightEntity* target;

	TCODConsole console;

	SoLoud::Wav torpedo_out;

	bool firing;
	float torpedo_timer;
	bool said;

	void fire_torpedo();
	bool has_torpedo();

	Targeting();
	~Targeting();

	// Ignores torpedoes, bases, nests and stations
	std::vector<FlightEntity*> get_visible_entities();

	// Inherited via Workbench
	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	virtual TCODConsole * get_console() override;

	virtual std::string get_name() override
	{
		return "Targeting Station";
	}
};

