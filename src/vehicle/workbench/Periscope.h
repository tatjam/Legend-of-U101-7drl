#pragma once

#include <string>
#include <vector>
#include "../../Date.h"
#include "libtcod.h"

#include "Workbench.h"
#include "../../Drawing.h"



class Periscope : public Workbench
{
private:

public:

	bool blink = false;
	float blinkt = 0.5f;

	TCODConsole console;

	Periscope();
	~Periscope();

	// Inherited via Workbench
	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	virtual TCODConsole* get_console() override
	{
		return &console;
	}

	virtual std::string get_name() override
	{
		return "Periscope";
	}
};

