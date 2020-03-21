#pragma once

#include <string>
#include <vector>
#include "../../Date.h"
#include "libtcod.h"

#include "Workbench.h"
#include "../../Drawing.h"

class Maneouver : public Workbench
{
private:
	float dt;

public:

	SoLoud::Wav engine_off;

	float wanted_angle;
	float wanted_velocity;

	bool said;
	bool said_speed;
	bool said_max;

	TCODConsole console;

	Maneouver();
	~Maneouver();

	// Inherited via Workbench
	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	virtual TCODConsole* get_console() override
	{
		return &console;
	}

	virtual std::string get_name() override
	{
		return "Maneouvering Station";
	}
};

