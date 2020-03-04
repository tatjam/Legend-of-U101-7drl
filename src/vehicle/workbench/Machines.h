#pragma once

#include <string>
#include <vector>
#include "../../Date.h"
#include "libtcod.h"

#include "Workbench.h"

class Machines : public Workbench
{
public:
	Machines();
	~Machines();

	// Inherited via Workbench
	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	virtual TCODConsole * get_console() override;

	virtual std::string get_name() override
	{
		return "Machine Control";
	}
};

