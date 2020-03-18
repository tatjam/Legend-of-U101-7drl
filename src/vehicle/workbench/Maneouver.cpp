#include "Maneouver.h"
#include "../Vehicle.h"


int angle_to_pixel(float angle)
{
	int i = (int)round(((angle + PI) / (2.0f * PI)) * 44) % 45;

	return i;
}

inline float wrap_angle(float angle)
{
	float two_pi = 2.0f * PI;
	return angle - two_pi * floor(angle / two_pi);
}

Maneouver::Maneouver() : console(50, 50)
{
	wanted_angle = 0.0f;
	engine_off.load("engine_off.wav");
	said = true;
	said_speed = true;
}


Maneouver::~Maneouver()
{
}

bool Maneouver::update(float dt)
{
	this->dt = dt;

	if (g_key.vk == TCODK_ESCAPE) 
	{
		return true;
	}

	// Update velocity
	float diff = wanted_velocity - get_vehicle()->velocity;
	if (fabs(diff) <= 0.0005) 
	{
		get_vehicle()->velocity = wanted_velocity;

		if (!said_speed)
		{
			if (is_crewed())
			{
				get_crewman()->gc->speak(Speech::speed_done());
			}

			said_speed = true;
		}
	}
	else
	{
		float v = diff > 0 ? 1.0f : -1.0f;
		float vel = diff > 0 ? 0.05f : 0.14f;
		get_vehicle()->velocity += v * dt * vel;

		said_speed = false;
	}

	float real_wanted = wrap_angle(wanted_angle);
	float real_vehicle = wrap_angle(get_vehicle()->angle);

	float dir = 1.0f;
	diff = real_wanted - real_vehicle;

	if (fabs(diff) <= 0.005)
	{
		get_vehicle()->angle = wanted_angle;

		if (!said) 
		{
			if (is_crewed())
			{
				get_crewman()->gc->speak(Speech::heading_done());
			}

			said = true;
		}
	}
	else
	{
		said = false;

		if (fabs(diff) > PI)
		{
			dir = -1.0f;
		}

		if (diff < 0)
		{
			dir = -dir;
		}


		float possible_maneouver = 1.0f - get_vehicle()->water_level / 50.0f;
		if (possible_maneouver < 0.2f) 
		{
			possible_maneouver = 0.2f;
		}

		get_vehicle()->angle = real_vehicle + dir * dt * 0.7f * possible_maneouver;

	}

	if (get_vehicle()->velocity > get_vehicle()->possible_speed && wanted_velocity > get_vehicle()->possible_speed
		&& get_vehicle()->velocity != 0.0)
	{
		get_vehicle()->velocity = get_vehicle()->possible_speed + 0.0000001f;

		if (!said_max)
		{
			if (is_crewed())
			{
				get_crewman()->gc->speak("Engine power too low!");
				said_max = true;
			}

		}
	}
	else 
	{
		said_max = false;
	}

	return false;
}

void Maneouver::draw(int rx, int ry)
{
	console.clear();

	// Compass with wanted and current heading

	int by = 8;
	Drawing::draw_rectangle(&console, 1, by, 46, 4, true);
	console.putChar(angle_to_pixel(wrap_angle(wanted_angle)) + 2, by + 1, 31);
	console.putChar(angle_to_pixel(wrap_angle(get_vehicle()->angle)) + 2, by + 3, 30);
	console.putChar(angle_to_pixel(0.0f) + 2, by + 2, 'N');
	console.putChar(angle_to_pixel(PI / 2.0f) + 2, by + 2, 'E');
	console.putChar(angle_to_pixel(-PI / 2.0f) + 2, by + 2, 'W');

	// Special one, south is in both sides...
	console.putChar(2, by + 2, 'S');
	console.putChar(44 + 2, by + 2, 'S');

	float fast = 1.0f;
	float slow = 0.05f;

	if (Drawing::draw_button(&console, 1, 4, rx, ry, 174))
	{
		wanted_angle -= dt * fast;
	}

	if (Drawing::draw_button(&console, 5, 4, rx, ry, '<'))
	{
		wanted_angle -= dt * slow;
	}

	if (Drawing::draw_button(&console, 9, 4, rx, ry, '>'))
	{
		wanted_angle += dt * slow;
	}

	if (Drawing::draw_button(&console, 13, 4, rx, ry, 175))
	{
		wanted_angle += dt * fast;
	}
	
	Drawing::draw_rectangle(&console, 49 - 6, 4, 4, 2);
	console.printf(49 - 5, 5, "%i", (int)round(wrap_angle(get_vehicle()->angle) / (2.0f * PI) * 360.0f));

	Drawing::draw_rectangle(&console, 49 - 12, 4, 4, 2);
	console.printf(49 - 11, 5, "%i", (int)round(wrap_angle(wanted_angle) / (2.0f * PI) * 360.0f));

	if (Drawing::draw_button(&console, 1, 17, rx, ry, 30))
	{
		wanted_velocity += dt * 0.2f;
	}

	if (Drawing::draw_button(&console, 5, 17, rx, ry, 31))
	{
		wanted_velocity -= dt * 0.2f;
	}

	if (Drawing::draw_button(&console, 49 - 4, 17, rx, ry, 'X'))
	{
		// Play sound
		g_soloud->play(engine_off, get_vehicle()->velocity * 2.0f);
		wanted_velocity = 0.0f;
	}

	if (wanted_velocity >= 1.0f)
	{
		wanted_velocity = 1.0f;
	}

	if (wanted_velocity <= 0.0f)
	{
		wanted_velocity = 0.0f;
	}


	Drawing::draw_rectangle(&console, 1, 20, 46, 3, true);
	for (int i = 0; i < wanted_velocity * 45; i++)
	{
		console.putChar(i + 2, 21, 254);
	}

	for (int i = 0; i < get_vehicle()->velocity * 45; i++)
	{
		console.putChar(i + 2, 22, 254);
	}

}
