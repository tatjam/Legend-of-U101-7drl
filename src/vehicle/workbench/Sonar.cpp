#include "Sonar.h"
#include "../Vehicle.h"


bool Sonar::update(float dt)
{
	sonar_radius += dt * 14.0f;

	blinkt -= dt;
	if (blinkt <= 0.0f) 
	{
		blinkt = 0.5f;
		blink = !blink;

		if (is_open())
		{
			//draw_world(get_vehicle()->get_tile().first, get_vehicle()->get_tile().second, *get_vehicle()->in_map);
		}
	}

	if (!sonar_active)
	{
		sonar_radius = 0.0f;
	}

	if (sonar_radius >= 40.0f)
	{
		sonar_radius = 0.0f;
		g_soloud->play(ping);

		draw_world((int)floor(get_vehicle()->x), (int)floor(get_vehicle()->y), *get_vehicle()->in_map);
	}



	if (is_open())
	{
		if (g_key.vk == TCODK_ESCAPE) 
		{
			return true;
		}
	}

	return false;
}

void Sonar::draw(int rx, int ry)
{
	if (!sonar_active)
	{
		draw_world((int)floor(get_vehicle()->x), (int)floor(get_vehicle()->y), *get_vehicle()->in_map);
	}

	// Clear lower area
	console.rect(0, 49, 49, 60, true);

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
	console.setChar(24, 24, '@');

	// Directional indicator
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

	if (Drawing::draw_switch(&console, 8, 51, rx, ry, &sonar_active, "Active Sonar"))
	{
		if (sonar_active)
		{
			g_soloud->play(ping);
			draw_world((int)floor(get_vehicle()->x), (int)floor(get_vehicle()->y), *get_vehicle()->in_map);
		}
	}

	console.setDefaultForeground(TCODColor::white);

	// Position indicator
	Drawing::draw_rectangle(&console, 20, 51, 8, 2);

	// Change the central line
	console.putChar(20 + 4, 51, 194);
	console.putChar(20 + 4, 52, 179);
	console.putChar(20 + 4, 53, 193);


	console.setDefaultForeground(TCODColor(128, 255, 0));
	
	auto coarse = get_vehicle()->get_tile();

	console.printf(21, 52, "%i", coarse.first);
	console.printf(25, 52, "%i", coarse.second);

	console.setDefaultForeground(TCODColor::white);

	console.setAlignment(TCOD_CENTER);

	console.printf(24, 50, "Coarse");

	console.setAlignment(TCOD_LEFT);

	// Subposition indicator
	Drawing::draw_rectangle(&console, 30, 51, 8, 2);

	// Change the central line
	console.putChar(30 + 4, 51, 194);
	console.putChar(30 + 4, 52, 179);
	console.putChar(30 + 4, 53, 193);


	console.setDefaultForeground(TCODColor(128, 255, 0));

	auto fine = get_vehicle()->get_subtile();

	console.printf(31, 52, "%i", (int)(fine.first * 1000));
	console.printf(35, 52, "%i", (int)(fine.second * 1000));

	console.setDefaultForeground(TCODColor::white);

	console.setAlignment(TCOD_CENTER);

	console.printf(34, 50, "Fine");

	console.setAlignment(TCOD_LEFT);

}

void Sonar::draw_world(int px, int py, FlightMap& map)
{
	lpx = px;
	lpy = py;
	last_map = &map;
	// Run a visibility algorithm and draw
	map.vmap.computeFov(px, py, sonar_active ? 24 : 1, true, FOV_DIAMOND);

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
				int ch = 0;

				switch (map.tiles[my * map.width + mx])
				{
				case CLEAR:
					ch = 176;
					break;
				case WALL:
					ch = 178;
					break;
				case AIR:
					ch = 177;
					break;
				case STATION:
					ch = 234;
					break;
				case NEST:
					ch = 15;
					break;
				case BASE:
					ch = 127;
					break;
				default:
					ch = 1;
					break;
				}

				console.setChar(sx, sy, ch);

			}
			else
			{
				console.setChar(sx, sy, 250);
				//radar.setChar(sx, sy, ' ');
			}

		}
	}

	// Draw entities
	for (int i = 0; i < map.entities.size(); i++)
	{
		if (map.entities[i]->is_alive())
		{
			auto type = map.entities[i]->get_type();
			if (type != E_BASE && type != E_STATION && type != E_NEST)
			{
				int ch = '*';

				if (type == E_SUBMARINE)
				{
					ch = '!';
				}
				else if (type == E_TORPEDO)
				{
					ch = '+';
				}


				if (map.entities[i]->is_identified())
				{
					ch = map.entities[i]->get_symbol();
				}

				auto fpos = map.entities[i]->get_position();
				int x = (int)floor(fpos.first);
				int y = (int)floor(fpos.second);

				int mx = x - px;
				int my = y - py;

				int sx = mx + 24;
				int sy = my + 24;

				bool seen = true;


				if (!map.vmap.isInFov(x, y))
				{
					seen = false;
				}

				if (seen)
				{
					console.setChar(sx, sy, ch);
					map.entities[i]->set_visible(true);
				}
				else
				{
					map.entities[i]->set_visible(false);
				}

			}
		}
	}
}



Sonar::Sonar(SoLoud::Soloud * soloud) : console(49, 56)
{
	this->soloud = soloud;
	ping.load("ping.wav");
	blink = true;
	blinkt = 0.5f;
	sonar_active = false;
}

Sonar::~Sonar()
{

}

