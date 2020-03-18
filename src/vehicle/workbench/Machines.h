#pragma once

#include <string>
#include <vector>
#include "../../Date.h"
#include "libtcod.h"

#include "Workbench.h"
#include "../../Drawing.h"


// If you unload a battery while it is loading
// it heats up and could cause a failure
// so strategic loading is neccesary
enum MachineOutput
{
	BATTERY_A,
	BOTH_BATTERIES,
	BATTERY_B,
	ENGINES
};

class Machines : public Workbench
{
private:

public:

	constexpr static float RATE = 0.04f;

	SoLoud::Wav diesel;
	SoLoud::handle diesel_h;
	
	SoLoud::Wav diesel_off;

	bool eng0_running;
	bool eng1_running;

	MachineOutput eng0_out;
	MachineOutput eng1_out;

	TCODConsole console;

	Machines();
	~Machines();

	// Inherited via Workbench
	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	virtual TCODConsole* get_console() override
	{
		return &console;
	}

	virtual std::string get_name() override
	{
		return "Diesel Engines";
	}
};

