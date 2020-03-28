#include "Buildings.h"
#include "../FlightScene.h"

void EntityNest::update(float dt)
{
	timer -= dt;
	float px = get_scene()->vehicle.x;
	float py = get_scene()->vehicle.y;

	float dx = px - get_x();
	float dy = py - get_y();

	float dist = sqrt(dx * dx + dy * dy);
	float thresold = 1.0f + get_scene()->vehicle.noise * 5.0f;

	if (dist <= thresold)
	{
		// Send attackers
		if (attackers > 0 && timer <= 0.0f && g_random->getFloat(0.0f, 1.0f) >= 0.98f)
		{
			attackers--;
			
			if (g_random->getFloat(0.0f, 1.0f) >= 0.5f)
			{
				// Biter
			}
			else
			{
				if (g_random->getFloat(0.0f, 1.0f) > 0.5f)
				{
					// Bomb
				}
				else
				{
					if (g_random->getFloat(0.0f, 1.0f) > 0.5f)
					{
						// Crawler
					}
					else
					{
						// Worm
					}
				}
			}

			timer = 5.0f;

		}
	}
}

EntityType EntityNest::get_type()
{
	return E_NEST;
}

int EntityNest::get_symbol()
{
	return 15;
}
