#include "Battery.h"
#include "../Vehicle.h"

static void charge_meter(TCODConsole* target, int x, int y, float charge, float max = 1.0f)
{
	Drawing::draw_rectangle(target, x, y, 30, 2);

	float v = charge / max;

	for (int i = 0; i < v * 29; i++)
	{
		target->setChar(x + i + 1, y + 1, 254);
	}
}

Battery::Battery() : console(50, 50)
{
	ba_to_bb = false;
	bb_to_ba = false;
	ba_to_aux = false;
	bb_to_aux = false;
	ba_to_eng = false;
	bb_to_eng = false;

}


Battery::~Battery()
{
}

bool Battery::update(float dt)
{
	get_vehicle()->possible_speed = 0.0f;

	float a_consume = 0.0f;
	float b_consume = 0.0f;
	float aux_consume = 0.0f;

	if (get_vehicle()->battery_a > 0.0f)
	{
		if (ba_to_eng)
		{
			get_vehicle()->possible_speed += 0.75f;
			a_consume += ENGINE_RATE * get_vehicle()->velocity;
		}

		if (ba_to_bb)
		{
			if (get_vehicle()->battery_b < 1.0f - CHARGE_RATE * dt)
			{
				a_consume += CHARGE_RATE;
				b_consume -= CHARGE_RATE;
			}
		}

		if (ba_to_aux)
		{
			if (get_vehicle()->battery_aux < 0.25f - CHARGE_RATE * dt)
			{
				a_consume += CHARGE_RATE;
				aux_consume -= CHARGE_RATE;
			}
		}
	}
	else
	{
		get_vehicle()->battery_a = 0.0f;
	}

	if (get_vehicle()->battery_b > 0.0f)
	{
		if (bb_to_eng)
		{
			get_vehicle()->possible_speed += 0.75f;
			b_consume += ENGINE_RATE * get_vehicle()->velocity;
		}

		if (bb_to_ba)
		{
			if (get_vehicle()->battery_a < 1.0f - CHARGE_RATE * dt)
			{
				b_consume += CHARGE_RATE;
				a_consume -= CHARGE_RATE;
			}
		}

		if (bb_to_aux)
		{
			if (get_vehicle()->battery_aux < 0.25f - CHARGE_RATE * dt)
			{
				b_consume += CHARGE_RATE;
				aux_consume -= CHARGE_RATE;
			}
		}
	}
	else
	{
		get_vehicle()->battery_b = 0.0f;
	}

	if (get_vehicle()->battery_aux > 0.0f)
	{
		if (aux_to_a)
		{
			if (get_vehicle()->battery_a < 1.0f - CHARGE_RATE * dt)
			{
				aux_consume += CHARGE_RATE;
				a_consume -= CHARGE_RATE;
			}
		}

		if (aux_to_b)
		{
			if (get_vehicle()->battery_b < 1.0f - CHARGE_RATE * dt)
			{
				aux_consume += CHARGE_RATE;
				b_consume -= CHARGE_RATE;
			}
		}

		if (aux_to_eng)
		{
			get_vehicle()->possible_speed += 0.25f;
			aux_consume += ENGINE_RATE * 0.5f * get_vehicle()->velocity;
		}
	}
	else
	{
		get_vehicle()->battery_aux = 0.0f;
	}

	get_vehicle()->battery_a -= a_consume * dt;
	get_vehicle()->battery_b -= b_consume * dt;
	get_vehicle()->battery_aux -= aux_consume * dt;

	if (get_vehicle()->battery_a >= 1.0f)
	{
		get_vehicle()->battery_a = 1.0f;
	}

	if (get_vehicle()->battery_b >= 1.0f)
	{
		get_vehicle()->battery_b = 1.0f;
	}

	if (get_vehicle()->battery_aux >= 0.25f)
	{
		get_vehicle()->battery_aux = 0.25f;
	}

	if (get_vehicle()->possible_speed >= 1.0f)
	{
		get_vehicle()->possible_speed = 1.0f;
	}

	get_vehicle()->possible_speed = get_vehicle()->engines_on_diesel ? 1.0f : get_vehicle()->possible_speed;

	return false;
}

void Battery::draw(int rx, int ry)
{
	console.clear();

	int by = 34;

	Drawing::draw_switch(&console, 7, by + 0, rx, ry, &ba_to_bb, "BAT.A > BAT.B");
	Drawing::draw_switch(&console, 7, by + 5, rx, ry, &ba_to_aux, "BAT.A > B.SHK");
	Drawing::draw_switch(&console, 7, by + 10, rx, ry, &ba_to_eng, "BAT.A > MOTOR");

	Drawing::draw_switch(&console, 23, by + 0, rx, ry, &bb_to_ba, "BAT.B > BAT.A");
	Drawing::draw_switch(&console, 23, by + 5, rx, ry, &bb_to_aux, "BAT.B > B.SHK");
	Drawing::draw_switch(&console, 23, by + 10, rx, ry, &bb_to_eng, "BAT.B > MOTOR");

	Drawing::draw_switch(&console, 39, by + 0, rx, ry, &aux_to_a, "B.SHK > BAT.A");
	Drawing::draw_switch(&console, 39, by + 5, rx, ry, &aux_to_b, "B.SHK > BAT.B");
	Drawing::draw_switch(&console, 39, by + 10, rx, ry, &aux_to_eng, "B.SHK > MOTOR");

	console.hline(0, by - 4, 50);
	console.vline(16, by - 4, 20);
	console.vline(32, by - 4, 20);

	console.setChar(16, by - 4, 194);
	console.setChar(32, by - 4, 194);

	// Draw indicators

	console.printf(2, 2, "BAT. ID");
	console.printf(20, 2, "CHARGE");
	console.printf(43, 2, "TEMP");

	console.printf(2, 6, "BAT.A");
	charge_meter(&console, 9, 5, get_vehicle()->battery_a);

	console.printf(2, 10, "BAT.B");
	charge_meter(&console, 9, 9, get_vehicle()->battery_b);

	console.printf(2, 14, "B.SHK");
	charge_meter(&console, 9, 13, get_vehicle()->battery_aux, 0.25f);
}

