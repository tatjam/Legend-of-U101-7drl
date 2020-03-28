#pragma once

#include "libtcod.h"
#include "soloud.h"
#include "soloud_wav.h"

#include "../../flight/FlightMap.h"
#include "../../Drawing.h"

#include "Workbench.h"

class Sonar : public Workbench
{
public:

	float blinkt;
	bool blink;

	int lpx, lpy;
	FlightMap* last_map;

	SoLoud::Soloud* soloud;

	TCODConsole console;
	float sonar_radius;
	bool sonar_active;
	SoLoud::Wav ping;

	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	void draw_world(int px, int py, FlightMap& map);

	virtual TCODConsole* get_console() override
	{
		return &console;
	}

	virtual std::string get_name() override
	{
		return "Sonar";
	}

	Sonar(SoLoud::Soloud* soloud);
	~Sonar();
};

