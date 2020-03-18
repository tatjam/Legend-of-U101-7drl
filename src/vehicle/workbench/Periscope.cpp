#include "Periscope.h"
#include "../Vehicle.h"
#include "../../flight/FlightScene.h"

Periscope::Periscope() : console(49, 49)
{
}


Periscope::~Periscope()
{
}

bool Periscope::update(float dt)
{
	if (g_key.vk == TCODK_ESCAPE)
	{
		return true;
	}

	blinkt -= dt;
	if (blinkt <= 0.0f) 
	{
		blinkt = 0.5f;
		blink = !blink;
	}

	return false;
}

void Periscope::draw(int rx, int ry)
{

	int w = 49;
	int h = 49;

	TCODMap view_map = TCODMap(w, h);

	std::vector<FlightEntity*>& entities = get_vehicle()->in_map->get_entities();

	float vx = get_vehicle()->x;
	float vy = get_vehicle()->y;

	vx = floor(vx * 20.0f) / 20.0f;
	vy = floor(vy * 20.0f) / 20.0f;

	for (int x = 0; x < w; x++) 
	{
		for (int y = 0; y < h; y++)
		{
			// xf, yf (-1, 1)
			float xf = (float)x / (float)w;
			float yf = (float)y / (float)h;
			xf = (xf - 0.5f) * 2.0f;
			yf = (yf - 0.5f) * 2.0f;

			float tx = vx + xf;
			float ty = vy + yf;

			int xr = (int)floor(tx);
			int yr = (int)floor(ty);

			MapTile in_tile = get_vehicle()->in_map->get_subtile(tx, ty);

			if (in_tile == WALL)
			{
				// Fill with rock
				console.putChar(x, y, '#');
				view_map.setProperties(x, y, false, false);
			}
			else
			{
				console.putChar(x, y, 250);
				view_map.setProperties(x, y, true, true);
			}


		}
	}


	// Draw entities and features
	for (int i = 0; i < entities.size(); i++) 
	{	
		if (entities[i]->is_alive())
		{
			float exf = entities[i]->get_position().first - vx;
			float eyf = entities[i]->get_position().second - vy;

			int ex = (int)floor(exf * (float)w * 0.5f + 24.0f);
			int ey = (int)floor(eyf * (float)h * 0.5f + 24.0f);

			if (ex >= 0 && ex < w && ey >= 0 && ey < h)
			{
				console.setChar(ex, ey, entities[i]->get_symbol());
			}
		}
	}

	view_map.computeFov(24, 24, w, true, FOV_BASIC);
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			float xf = (float)x / (float)w;
			float yf = (float)y / (float)h;
			xf = (xf - 0.5f) * 2.0f;
			yf = (yf - 0.5f) * 2.0f;

			float dist = sqrt(xf * xf + yf * yf) * 7.0f;
			float bright = 3.0f / (dist * dist);


			TCODColor lit = TCODColor(207.0f, 1.0f / bright, bright);

			if (view_map.isInFov(x, y))
			{
				console.setCharForeground(x, y, lit);
			}
			else
			{
				console.setCharForeground(x, y, TCODColor::black);
			}
		}
	}

	console.putChar(24, 24, '@');

	if (blink)
	{
		float a = get_vehicle()->angle;
		float p = PI / 8.0f;

		if (a <= 1.0f * p)
		{
			//N (0-22.5)
			console.setCharForeground(24, 23, TCODColor::lightGrey);
			console.setChar(24, 23, 179);
		}
		else if (a <= 3.0f * p)
		{
			//NE (22.5-45)
			console.setCharForeground(25, 23, TCODColor::lightGrey);
			console.setChar(25, 23, 47);
		}
		else if (a <= 5.0f * p)
		{
			// E (45-67.5)
			console.setCharForeground(25, 24, TCODColor::lightGrey);
			console.setChar(25, 24, 196);
		}
		else if (a <= 7.0f * p)
		{
			// SE (67.5-90)
			console.setCharForeground(25, 25, TCODColor::lightGrey);
			console.setChar(25, 25, 92);
		}
		else if (a <= 9.0f * p)
		{
			// S
			console.setCharForeground(24, 25, TCODColor::lightGrey);
			console.setChar(24, 25, 179);
		}
		else if (a <= 11.0f * p)
		{
			// SW
			console.setCharForeground(23, 25, TCODColor::lightGrey);
			console.setChar(23, 25, 47);
		}
		else if (a <= 13.0f * p)
		{
			// W
			console.setCharForeground(23, 24, TCODColor::lightGrey);
			console.setChar(23, 24, 196);
		}
		else if (a <= 15.0f * p)
		{
			// NW
			console.setCharForeground(23, 23, TCODColor::lightGrey);
			console.setChar(23, 23, 92);
		}
		else
		{
			// N
			console.setCharForeground(24, 23, TCODColor::lightGrey);
			console.setChar(24, 23, 179);
		}
	}

	// Draw explosions
	for (int i = 0; i < get_vehicle()->scene->expl_effects.size(); i++)
	{
		ExplosionEffect fx = get_vehicle()->scene->expl_effects[i];
	

		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				// xf, yf (-1, 1)
				float xf = (float)x / (float)w;
				float yf = (float)y / (float)h;
				xf = (xf - 0.5f) * 2.0f;
				yf = (yf - 0.5f) * 2.0f;

				float tx = vx + xf;
				float ty = vy + yf;

				float dx = tx - fx.x;
				float dy = ty - fx.y;

				float dist = sqrt(dx * dx + dy * dy);

				if (dist <= fx.t * 2.0f)
				{
					int n = min(fx.t * 255.0f, 255);
					console.setCharForeground(x, y, TCODColor(n, n, n));
					if (dist <= fx.t)
					{
						console.setChar(x, y, 178);
					}
					else
					{
						console.setChar(x, y, '.');
					}
					
				}
			}
		}
	}

}
