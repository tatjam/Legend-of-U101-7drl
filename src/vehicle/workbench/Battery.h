#pragma once

#include <string>
#include <vector>
#include "../../Date.h"
#include "libtcod.h"

#include "Workbench.h"
#include "../../Drawing.h"


class Battery : public Workbench
{
private:

public:

	static constexpr float ENGINE_RATE = 0.007f;
	static constexpr float CHARGE_RATE = 0.050f;

	bool ba_to_bb;
	bool bb_to_ba;
	bool ba_to_aux;
	bool bb_to_aux;
	bool ba_to_eng;
	bool bb_to_eng;

	bool aux_to_a;
	bool aux_to_b;
	bool aux_to_eng;

	TCODConsole console;

	Battery();
	~Battery();

	// Inherited via Workbench
	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	virtual TCODConsole* get_console() override
	{
		return &console;
	}

	virtual std::string get_name() override
	{
		return "Battery Station";
	}
};

