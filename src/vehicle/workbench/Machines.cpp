#include "Machines.h"
#include "../Vehicle.h"


Machines::Machines() : console(40, 50)
{
	eng0_running = false;
	eng1_running = false;
	eng0_out = BOTH_BATTERIES;
	eng1_out = ENGINES;

	diesel.load("diesel.wav");
	diesel.setLooping(true);

	diesel_off.load("diesel_off.wav");

	diesel_h = g_soloud->play(diesel, 0.0f);
}


Machines::~Machines()
{
}

bool Machines::update(float dt)
{
	if (!get_vehicle()->breathing)
	{
		if (eng0_running || eng1_running)
		{
			// Play starving sound
			g_soloud->play(diesel_off, 2.0f);
		}

		eng0_running = false;
		eng1_running = false;
	}

	if (eng0_running || eng1_running)
	{
		g_soloud->setVolume(diesel_h, 1.0f);
	}
	else
	{
		g_soloud->setVolume(diesel_h, 0.0f);
	}

	float to_bat_a = 0.0f;
	float to_bat_b = 0.0f;
	float to_engines = 0.0f;

	if (eng0_running)
	{
		if (eng0_out == BATTERY_A) 
		{
			to_bat_a += 0.5f;
		}
		else if (eng0_out == BATTERY_B) 
		{
			to_bat_b += 0.5f;
		}
		else if (eng0_out == BOTH_BATTERIES)
		{
			to_bat_a += 0.25f;
			to_bat_b += 0.25f;
		}
		else
		{
			to_engines += 0.5f;
		}
	}

	if (eng1_running)
	{
		if (eng1_out == BATTERY_A)
		{
			to_bat_a += 0.5f;
		}
		else if (eng1_out == BATTERY_B)
		{
			to_bat_b += 0.5f;
		}
		else if (eng1_out == BOTH_BATTERIES)
		{
			to_bat_a += 0.25f;
			to_bat_b += 0.25f;
		}
		else
		{
			to_engines += 0.5f;
		}
	}

	get_vehicle()->battery_a += to_bat_a * dt * RATE;
	get_vehicle()->battery_b += to_bat_b * dt * RATE;

	if (get_vehicle()->battery_a > 1.0f)
	{
		get_vehicle()->battery_a = 1.0f;
	}

	if (get_vehicle()->battery_b > 1.0f)
	{
		get_vehicle()->battery_b = 1.0f;
	}


	get_vehicle()->engines_on_diesel = to_engines != 0.0f;

	return false;
}

void Machines::draw(int rx, int ry)
{
	console.clear();

	console.setAlignment(TCOD_CENTER);

	if (get_vehicle()->breathing)
	{
		console.setDefaultForeground(TCODColor::lightGreen);
		console.printf(console.getWidth() / 2 - 1, 2, "OXYGEN AVAILABLE");
		console.printf(console.getWidth() / 2 - 1, 4, "ENGINES READY");
	}
	else
	{
		console.setDefaultForeground(TCODColor::lightRed);
		console.printf(console.getWidth() / 2 - 1, 2, "OXYGEN UNAVAILABLE");
		console.printf(console.getWidth() / 2 - 1, 4, "ENGINES CANNOT RUN");
	}

	console.setDefaultForeground(TCODColor::white);

	console.setAlignment(TCOD_LEFT);

	console.hline(0, 6, console.getWidth());

	int y0 = 12;
	console.setAlignment(TCOD_CENTER);
	console.printf(console.getWidth() / 2 - 1, y0 - 5, "Generator A");
	console.setAlignment(TCOD_LEFT);

	if (Drawing::draw_switch(&console, 9, y0 + 3, rx, ry, &eng0_running, "Generator Active") && eng0_running == false)
	{
			g_soloud->play(diesel_off, 2.0f);
	}

	Drawing::draw_four_choice(&console, 26, y0, rx, ry, (int*)&eng0_out, "BAT A", "BATTS", "BAT B", "MOTOR");

	y0 = console.getHeight() / 2 + 6;

	console.setAlignment(TCOD_CENTER);
	console.printf(console.getWidth() / 2 - 1, y0 - 5, "Generator B");
	console.setAlignment(TCOD_LEFT);

	if (Drawing::draw_switch(&console, 9, y0 + 3, rx, ry, &eng1_running, "Generator Active") && eng0_running == false)
	{
		g_soloud->play(diesel_off, 2.0f);
	}

	Drawing::draw_four_choice(&console, 26, y0, rx, ry, (int*)&eng1_out, "BAT A", "BATTS", "BAT B", "MOTOR");


	console.hline(0, console.getHeight() / 2, console.getWidth());
}
