#include "Sonar.h"



void Sonar::update(float dt)
{
	sonar_radius += dt * 14.0f;

	if (!sonar_active)
	{
		sonar_radius = 0.0f;
	}

	if (sonar_radius >= 40.0f)
	{
		sonar_radius = 0.0f;
		g_soloud->play(ping);
	}
}

void Sonar::draw(int rx, int ry)
{
	// Fade old pixels
	for (int x = 0; x < 49; x++)
	{
		for (int y = 0; y < 49; y++)
		{
			TCODColor cur = console.getCharForeground(x, y);

			cur.setValue(cur.getValue() * 0.984f);


			console.setCharForeground(x, y, cur);
		}
	}



	for (int x = 0; x < 49; x++)
	{
		for (int y = 0; y < 49; y++)
		{
			float xf = (float)(x - 24.0f);
			float yf = (float)(y - 24.0f);
			float dist = sqrt(xf * xf + yf * yf);
			if (abs(dist - sonar_radius) <= 2.0f)
			{
				console.setCharForeground(x, y, TCODColor(80, 255, 80));
			}
		}
	}



	console.setCharForeground(24, 24, TCODColor(255, 255, 255));
	console.setChar(24, 24, 127);

	if (Drawing::draw_switch(&console, 8, 51, rx, ry, &sonar_active, "Active Sonar") && sonar_active)
	{
		g_soloud->play(ping);
	}
}

void Sonar::draw_world(int px, int py, FlightMap& map)
{
	// Run a visibility algorithm and draw
	map.vmap.computeFov(px, py, sonar_active ? 24 : 4, true, FOV_BASIC);

	for (int x = -24; x < 24; x++)
	{
		for (int y = -24; y < 24; y++)
		{
			int mx = x + px;
			int my = y + py;

			int sx = x + 24;
			int sy = y + 24;

			bool seen = true;

			if (mx < 0 || my < 0 || mx >= map.width || my >= map.height)
			{
				seen = false;
			}

			if (!map.vmap.isInFov(mx, my))
			{
				seen = false;
			}

			if (seen)
			{
				if (map.tiles[my * map.width + mx].wall)
				{
					//console.setChar(sx, sy, 219); // Block
					console.setChar(sx, sy, 178);
				}
				else
				{
					//radar.setChar(sx, sy, 249);
					console.setChar(sx, sy, 176); // Dust style
				}

			}
			else
			{
				console.setChar(sx, sy, 250);
				//radar.setChar(sx, sy, ' ');
			}

		}
	}
}



Sonar::Sonar(SoLoud::Soloud * soloud) : console(49, 56)
{
	this->soloud = soloud;
	ping.load("ping.wav");
}

Sonar::~Sonar()
{

}

