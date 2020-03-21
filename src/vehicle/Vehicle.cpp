#include "Vehicle.h"
#include "../flight/FlightScene.h"


VehicleTile& Vehicle::get_tile_for_water(int x, int y)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return dummy_tile;
	}
	else
	{
		return tiles[y * width + x];
	}
}

void Vehicle::update_water_flow(float dt)
{
	water_tick -= dt;
	if (water_tick < 0.0f)
	{
		water_level = 0.0f;
		water_flow = 0.0f;
		// Update water flow
		// We set flowing here
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (tiles[y * width + x].is_outside)
				{
					tiles[y * width + x].health = 1.0f;
				}

				water_level += tiles[y * width + x].water;

				tiles[y * width + x].really_blocks_water = tiles[y * width + x].blocks_water;

				if (!tiles[y * width + x].is_outside)
				{
					tiles[y * width + x].prev_water = tiles[y * width + x].water;
				}

				if (tiles[y * width + x].blocks_water)
				{
					if (tiles[y * width + x].health <= 0.5f)
					{
						tiles[y * width + x].really_blocks_water = false;
					}

					bool found = false;

					for (int i = 0; i < crew.size(); i++)
					{
						if (crew[i].x == x && crew[i].y == y)
						{
							tiles[y * width + x].really_blocks_water = false;
							found = true;
							break;
						}
					}

					if (!found && tiles[y * width + x].health >= 0.5f)
					{
						tiles[y * width + x].water = 0.0f;
					}
				}
			}
		}

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				VehicleTile& c = get_tile_for_water(x + 0, y + 0);
				VehicleTile& u = get_tile_for_water(x + 0, y - 1);
				VehicleTile& r = get_tile_for_water(x + 1, y + 0);
				VehicleTile& d = get_tile_for_water(x + 0, y + 1);
				VehicleTile& l = get_tile_for_water(x - 1, y + 0);

				if (c.health <= 0.5f)
				{
					water_flow += 1.0f - c.health;
					if (g_random->getFloat(0.0f, 0.5f) > c.health)
					{
						c.water += g_random->getFloat(0.001f, 1.0f - c.health) * dt * 0.5f;
					}
				}

				if (!c.is_outside && !c.really_blocks_water)
				{
					for (int i = 0; i < 4; i++)
					{
						if (g_random->getFloat(0.0f, 1.0f) > 0.5f)
						{
							if (g_random->getFloat(0.0f, 1.0f) > 0.5f)
							{
								if (!(u.really_blocks_water && u.health >= 0.5f) && u.prev_water < c.prev_water)
								{
									u.water += min(WATER_FLOW_RATE * dt, c.water);
									c.water -= min(WATER_FLOW_RATE * dt, c.water);
								}
							}
							else
							{
								if (!(l.really_blocks_water && l.health >= 0.5f) && l.prev_water < c.prev_water)
								{
									l.water += min(WATER_FLOW_RATE * dt, c.water);
									c.water -= min(WATER_FLOW_RATE * dt, c.water);
								}
							}
						}
						else
						{
							if (g_random->getFloat(0.0f, 1.0f) > 0.5f)
							{
								if (!(r.really_blocks_water && r.health >= 0.5f) && r.prev_water < c.prev_water)
								{
									r.water += min(WATER_FLOW_RATE * dt, c.water);
									c.water -= min(WATER_FLOW_RATE * dt, c.water);
								}
							}
							else
							{
								if (!(d.really_blocks_water && d.health >= 0.5f) && d.prev_water < c.prev_water)
								{
									d.water += min(WATER_FLOW_RATE * dt, c.water);
									c.water -= min(WATER_FLOW_RATE * dt, c.water);
								}
							}
						}

					}


				}
			}
		}

		water_tick = 0.05f;
		g_soloud->setVolume(waterflow_h, min(water_flow / 10.0f, 2.0f));
	}


}

void Vehicle::update_crew(float dt)
{
	for (int i = 0; i < crew.size(); i++)
	{
		crew[i].path_t -= dt;

		if (crew[i].path != nullptr && crew[i].path_t <= 0.0f)
		{
			int nx, ny;
			crew[i].path->walk(&crew[i].x, &crew[i].y, true);

			crew[i].path_t = 0.1f;
		}

		if (crew[i].has_torpedo)
		{
			// Find free torpedo slot
			int slot = -1;
			for (int j = 0; j < torpedo_slots.size(); j++)
			{
				if (torpedo_slots[j].has_torpedo == false &&
					(torpedo_slots[j].crew_coming == i || torpedo_slots[j].crew_coming == -1))
				{
					slot = j;
					break;
				}
			}

			if (slot != -1)
			{
				torpedo_slots[slot].crew_coming = i;

				crew[i].path_to(torpedo_slots[slot].x, torpedo_slots[slot].y, *tcod_map);

				if (crew[i].x == torpedo_slots[slot].x && crew[i].y == torpedo_slots[slot].y)
				{
					torpedo_slots[slot].crew_coming = -1;
					torpedo_slots[slot].has_torpedo = true;
					crew[i].has_torpedo = false;

					crew[i].gc->speak("Torpedo loaded, sir!");
				}
			}
			else
			{
				// Drop the torpedo
				Torpedo tp = Torpedo();
				tp.x = crew[i].x;
				tp.y = crew[i].y;
				crew[i].has_torpedo = false;
				torpedoes.push_back(tp);
			}
		}
		else
		{
			if (crew[i].repair_order && !crew[i].is_repairing)
			{
				bool found = false;

				int tries = 0;

				while(!found && tries < 30)
				{
					int x = g_random->getInt(0, width - 1);
					int y = g_random->getInt(0, height - 1);

					if (tiles[y * width + x].health < 1.0f)
					{
						crew[i].repair_x = x;
						crew[i].repair_y = y;
						found = true;
					}

					tries++;
				}
			}

			if (crew[i].wants_torpedo)
			{
				int to_remove = -1;
				for (int j = 0; j < torpedoes.size(); j++)
				{
					if (crew[i].x >= torpedoes[j].x && crew[i].x <= torpedoes[j].x + 4 && crew[i].y == torpedoes[j].y)
					{
						to_remove = j;
						crew[i].wants_torpedo = false;
						crew[i].has_torpedo = true;
						break;
					}
				}

				if (to_remove != -1)
				{
					torpedoes.erase(torpedoes.begin() + to_remove);
				}
			}

			if (tiles[crew[i].y * width + crew[i].x].ch == 235)
			{
				crew[i].is_pumping = true;

				tiles[crew[i].y * width + crew[i].x].water -= WATER_PUMP_RATE * dt;
				if (tiles[crew[i].y * width + crew[i].x].water < 0.0f)
				{
					tiles[crew[i].y * width + crew[i].x].water = 0.0f;
					crew[i].is_pumping = false;
				}
			}
			else
			{
				crew[i].is_pumping = false;
			}

			if (crew[i].repair_x != -1)
			{
				if (crew[i].x >= crew[i].repair_x - 2
					&& crew[i].y >= crew[i].repair_y - 2
					&& crew[i].x <= crew[i].repair_x + 2
					&& crew[i].y <= crew[i].repair_y + 2)
				{
					crew[i].is_repairing = true;

					// Repair
					VehicleTile& to_repair = tiles[crew[i].repair_y * width + crew[i].repair_x];

					to_repair.health += REPAIR_RATE * dt;

					if (to_repair.health >= 1.0f)
					{
						to_repair.health = 1.0f;
						crew[i].repair_x = -1;
						crew[i].repair_y = -1;
						if (!crew[i].repair_order)
						{
							crew[i].gc->speak("Tile repaired!");
						}

						crew[i].is_repairing = false;

						if (crew[i].repair_order)
						{
							// Go to workstation
							crew[i].path_to(crew[i].cx, crew[i].cy, *tcod_map);
						}
					}
				}
				else
				{
					TCODPath t_path = TCODPath(tcod_map);
					// Path to any free tile
					for (int ox = -2; ox < 2; ox++)
					{
						for (int oy = -2; oy < 2; oy++)
						{
							if (t_path.compute(crew[i].x, crew[i].y, crew[i].repair_x + ox, crew[i].repair_y + oy))
							{
								crew[i].path_to(crew[i].repair_x + ox, crew[i].repair_y + oy, *tcod_map);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void Vehicle::update_bubbles(float dt)
{


	while (bubbles.size() <= 50)
	{
		float y = g_random->getFloat(0.0f, 40.0f);
		Bubble b = Bubble();

		b.x = 0.0f;
		b.y = y;

		bubbles.push_back(b);
	}

	for (auto it = bubbles.begin(); it != bubbles.end();)
	{
		it->t += dt;
		it->x += dt * velocity * 10.0f;

		if (g_random->getFloat(0.0f, 1.0f) >= 0.99f)
		{
			it->y += g_random->getInt(-1, 1);
		}


		it->b = (sin(it->t * it->f) + 1.0f) * 30.0f;

		if (it->x >= WIDTH)
		{
			it = bubbles.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void Vehicle::do_context_menu(TCODConsole* target, int ox, int oy, TCOD_mouse_t pos, int torpedo_hlight)
{
	if (selected != nullptr && pos.rbutton_pressed)
	{
		in_context_menu = true;
		ctx_x = pos.cx - ox;
		ctx_y = pos.cy - oy;

		if (ctx_x < 0 || ctx_x >= width || ctx_y < 0 || ctx_y >= height)
		{
			in_context_menu = false;
		}
	}

	if (selected != nullptr)
	{
		if (pos.mbutton_pressed || (pos.lbutton_pressed && (g_key.lctrl || g_key.rctrl)))
		{
			selected->path_to(pos.cx - ox, pos.cy - oy, *tcod_map);
			selected->is_repairing = false;
			selected->is_pumping = false;
		}
	}

	if (in_context_menu && selected)
	{
		if (ctx_x < 0 || ctx_x >= width || ctx_y < 0 || ctx_y >= height)
		{
			ctx_x = 0;
			ctx_y = 0;
		}

		int rx = ctx_x + ox;
		int ry = ctx_y + oy;

		if (blink)
		{
			target->setChar(rx, ry, 'X');
			target->setCharForeground(ctx_x + ox, ctx_y + oy, TCODColor::white);
		}

		std::vector<std::string> menu_items;
		if (!tiles[ctx_y * width + ctx_x].blocks_player)
		{
			menu_items.push_back("Move Here");
			menu_items.push_back("Assign Location");
		}

		if (torpedo_hlight != -1)
		{
			menu_items.push_back("Load Torpedo");
		}

		if (tiles[ctx_y * width + ctx_x].ch == 235)
		{
			menu_items.push_back("Pump Water Out");
		}

		// We can repair tiles in a 2x2 range
		if (tiles[ctx_y * width + ctx_x].health < 1.0f)
		{
			menu_items.push_back("Repair Tile");
		}

		if (selected->repair_order)
		{
			menu_items.push_back("Stop Auto-Repairing");
		}
		else
		{
			menu_items.push_back("Auto-Repair");
		}



		menu_items.push_back(selected->gc->name);



		int menu_x, menu_y;
		int menu_w = 0, menu_h = menu_items.size() + 1;

		for (int i = 0; i < menu_items.size(); i++)
		{
			if (menu_items[i].size() >= menu_w)
			{
				menu_w = menu_items[i].size();
			}
		}

		menu_w += 3;

		// Draw menu, choose direction carefully
		if (rx >= WIDTH - menu_w - 1)
		{
			menu_x = rx - menu_w - 1;
		}
		else
		{
			menu_x = rx + 1;
		}

		if (ry >= HEIGHT - menu_h - 1)
		{
			menu_y = ry - menu_h - 1;
		}
		else
		{
			menu_y = ry;
		}

		for (int x = menu_x; x < menu_x + menu_w + 1; x++)
		{
			for (int y = menu_y; y < menu_y + menu_h + 1; y++)
			{
				target->setChar(x, y, ' ');
				target->setCharForeground(x, y, TCODColor::white);
				target->setCharBackground(x, y, TCODColor::black);
			}
		}

		Drawing::draw_rectangle(target, menu_x, menu_y, menu_w, menu_h, false);

		target->setAlignment(TCOD_CENTER);

		for (int i = 0; i < menu_items.size(); i++)
		{
			bool highlight = false;

			if (pos.cx >= menu_x && pos.cx <= menu_x + menu_w
				&& pos.cy == menu_y + 1 + i)
			{
				highlight = true;
			}

			if (highlight)
			{
				target->setDefaultForeground(TCODColor::black);
				target->setDefaultBackground(TCODColor::white);
			}

			for (int x0 = 1; x0 < menu_w; x0++)
			{
				target->setCharBackground(menu_x + x0, menu_y + i + 1, target->getDefaultBackground());
			}

			target->printf(menu_x + menu_w / 2, menu_y + 1 + i, menu_items[i].c_str());

			target->setDefaultForeground(TCODColor::white);
			target->setDefaultBackground(TCODColor::black);

			if (highlight && (pos.lbutton_pressed && !(g_key.lctrl || g_key.rctrl)))
			{
				if (menu_items[i] == "Move Here")
				{
					selected->path_to(ctx_x, ctx_y, *tcod_map);
					selected->is_repairing = false;
					selected->is_pumping = false;
				}
				else if (menu_items[i] == "Load Torpedo")
				{
					selected->path_to(ctx_x, ctx_y, *tcod_map);
					selected->wants_torpedo = true;
					selected->is_repairing = false;
					selected->is_pumping = false;
				}
				else if (menu_items[i] == "Pump Water Out")
				{
					selected->path_to(ctx_x, ctx_y, *tcod_map);
				}
				else if (menu_items[i] == "Repair Tile")
				{
					selected->repair_x = ctx_x;
					selected->repair_y = ctx_y;
				}
				else if (menu_items[i] == "Auto-Repair")
				{
					selected->repair_order = true;
				}
				else if (menu_items[i] == "Stop Auto-Repairing")
				{
					selected->repair_order = false;
				}
				else if (menu_items[i] == "Assign Location")
				{
					selected->cx = ctx_x;
					selected->cy = ctx_y;
				}
			}

		}

		if ((pos.lbutton_pressed && !(g_key.lctrl || g_key.rctrl)))
		{
			in_context_menu = false;
		}

		target->setAlignment(TCOD_LEFT);
	}
}



FlightCrew * Vehicle::get_captain()
{
	for (int i = 0; i < crew.size(); i++)
	{
		if (crew[i].gc->is_captain)
		{
			return &crew[i];
		}
	}

	return nullptr;
}

void Vehicle::update(float dt)
{

	for (size_t i = 0; i < workbenches.size(); i++)
	{
		if (workbenches[i]->update(dt))
		{
			workbenches[i]->close();
			workbench_open = nullptr;
		}
	}

	auto old = get_tile();
	bool was_breathing = breathing;
	
	float slowdown = 1.0f - min(water_level * 0.025f, 1.0f);

	//std::cout << slowdown << std::endl;

	float dx = sin(angle) * velocity * dt * 0.2f * possible_speed;
	float dy = -cos(angle) * velocity * dt * 0.2f * possible_speed;

	if (in_map->get_subtile(x + dx, y + dy) == WALL)
	{
		if (velocity > 0.2f)
		{
			damage(1.5f);
		}

		velocity = 0.0f;
	}
	else
	{
		x += dx;
		y += dy;
	}

	noise = 0.0f;

	// Collect noise
	noise += velocity * 0.6f;
	noise += machines->eng0_running ? 0.6f : 0.0f;
	noise += machines->eng1_running ? 0.6f : 0.0f;
	noise += sonar->sonar_active ? 0.3f : 0.0f;
	noise /= 1.5f;

	if (get_tile() != old)
	{
		// Check for tile update
		MapTile in_now = in_map->get_tile(get_tile().first, get_tile().second);
		if (in_now == AIR)
		{
			breathing = true;
			if (!was_breathing)
			{
				if (maneouver->is_crewed())
				{
					maneouver->get_crewman()->gc->speak("We have entered an oxygen rich zone.");
				}
			}
		}
		else
		{
			breathing = false;
			if (was_breathing)
			{
				if (maneouver->is_crewed())
				{
					maneouver->get_crewman()->gc->speak("We have exited the oxygen rich zone.");
				}
			}
		}
	}

	float base_vol = 0.8f;
	float tri = max(-fabs(2.0f * velocity - 1.0f) + 1.0f, 0.0f);
	float tri2 = max(-fabs(2.0f * velocity - 2.0f) + 1.0f, 0.0f);

	g_soloud->setVolume(engines_low_h, tri * 0.25 * base_vol);
	g_soloud->setVolume(engines_high_h, tri2 * 0.8f * base_vol);
	g_soloud->setVolume(moving_slow_h, tri * 0.4f * base_vol);
	g_soloud->setVolume(moving_fast_h, velocity * base_vol);


	flip_timer -= velocity * dt * 10.0f;

	if (flip_timer <= 0.0f)
	{
		tiles[m0y * width + m0x].ch = tiles[m0y * width + m0x].ch == 192 ? 218 : 192;
		tiles[m1y * width + m1x].ch = tiles[m1y * width + m1x].ch == 192 ? 218 : 192;

		flip_timer = 1.0f;
	}

	blinkt -= dt;
	if (blinkt <= 0.0f)
	{
		blinkt = 0.5f;
		blink = !blink;
	}

	
	update_bubbles(dt);
	update_crew(dt);
	update_water_flow(dt);

}

void draw_torpedo(TCODConsole* target, int x, int y, int ox, int oy, bool hlight)
{
	target->setChar(x + ox, y + oy, 238);
	target->setChar(x + 1 + ox, y + oy, TCOD_CHAR_DHLINE);
	target->setChar(x + 2 + ox, y + oy, TCOD_CHAR_DHLINE);
	target->setChar(x + 3 + ox, y + oy, TCOD_CHAR_DHLINE);
	target->setChar(x + 4 + ox, y + oy, 232);
	target->setCharForeground(x + ox, y + oy, TCODColor(140, 70, 0));
	target->setCharForeground(x + 1 + ox, y + oy, TCODColor(158, 158, 158));
	target->setCharForeground(x + 2 + ox, y + oy, TCODColor(158, 158, 158));
	target->setCharForeground(x + 3 + ox, y + oy, TCODColor(158, 158, 158));
	target->setCharForeground(x + 4 + ox, y + oy, TCODColor(100, 100, 100));
	if (hlight)
	{
		for (int i = 0; i < 5; i++)
		{
			target->setCharBackground(x + ox + i, y + oy, TCODColor::white);
		}
	}
}

void Vehicle::draw(TCODConsole* target, int ox, int oy)
{
	TCOD_mouse_t pos = TCODMouse::getStatus();


	for (int i = 0; i < bubbles.size(); i++)
	{
		target->setChar(bubbles[i].x, bubbles[i].y, bubbles[i].ch);
		TCODColor col = TCODColor((int)bubbles[i].b, 77 + (int)bubbles[i].b, 102 + (int)bubbles[i].b);
		target->setCharForeground(bubbles[i].x, bubbles[i].y, col);
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (!(tiles[y * width + x].is_outside && tiles[y * width + x].ch != ']'))
			{
				if (tiles[y * width + x].water >= 0.4f && tiles[y * width + x].ch != 235)
				{
					target->setChar(x + ox, y + oy, 247);
					target->setCharForeground(x + ox, y + oy, TCODColor(0, 89, 178));
					target->setCharBackground(x + ox, y + oy, TCODColor(0, 32, 64));
				}
				else if (tiles[y * width + x].water > 0.25f && tiles[y * width + x].ch != 235)
				{
					target->setChar(x + ox, y + oy, 126);
					target->setCharForeground(x + ox, y + oy, TCODColor(0, 89, 178));
					target->setCharBackground(x + ox, y + oy, tiles[y * width + x].back);
				}
				else if (tiles[y * width + x].water > 0.0f && tiles[y * width + x].ch != 235 
					&& tiles[y * width + x].health == 1.0f)
				{
					target->setChar(x + ox, y + oy, 126);
					target->setCharForeground(x + ox, y + oy, TCODColor(51, 153, 255));
					target->setCharBackground(x + ox, y + oy, tiles[y * width + x].back);
				}
				else
				{
					target->setChar(x + ox, y + oy, tiles[y * width + x].ch);
					target->setCharForeground(x + ox, y + oy, tiles[y * width + x].fore);
					target->setCharBackground(x + ox, y + oy, tiles[y * width + x].back);

					if (tiles[y * width + x].health <= 0.05f)
					{
						target->setChar(x + ox, y + oy, tiles[y * width + x].blocks_player ? 8 : ',');
						target->setCharForeground(x + ox, y + oy, TCODColor(158, 158, 158));
						target->setCharBackground(x + ox, y + oy, TCODColor(80, 80, 80));
					}
					else if (tiles[y * width + x].health <= 0.25f)
					{
						target->setChar(x + ox, y + oy, tiles[y * width + x].blocks_player ? 10 : '%');
						target->setCharForeground(x + ox, y + oy, TCODColor(158, 158, 158));
					}
					else if (tiles[y * width + x].health <= 0.65f) 
					{
						target->setChar(x + ox, y + oy, tiles[y * width + x].blocks_player ? 15 : '+');
						target->setCharForeground(x + ox, y + oy, TCODColor(158, 158, 158));
					}
				}

			}
		}
	}

	int torpedo_hlight = -1;

	// Draw torpedoes
	for (int i = 0; i < torpedoes.size(); i++)
	{
		Torpedo& tp = torpedoes[i];

		bool hlight = false;
		if (selected != nullptr) 
		{
			int x = ctx_x; 
			int y = ctx_y;

			if (!in_context_menu)
			{
				x = pos.cx - ox;
				y = pos.cy - oy;
			}

			if (x >= tp.x && x <= tp.x + 4 && y == tp.y)
			{
				hlight = true;
				torpedo_hlight = i;
			}
			
			
		}


		draw_torpedo(target, tp.x, tp.y, ox, oy, hlight);

	}

	for (int i = 0; i < torpedo_slots.size(); i++)
	{
		if (torpedo_slots[i].has_torpedo)
		{
			target->setChar(torpedo_slots[i].x + ox, torpedo_slots[i].y + oy, 238);
			target->setCharForeground(torpedo_slots[i].x + ox, torpedo_slots[i].y + oy, TCODColor(140, 70, 0));
		}
	}

	// Draw crew
	bool selected_crew = false;
	for (int i = 0; i < crew.size(); i++)
	{
		FlightCrew& c = crew[i];
		
		int ch = c.gc->is_captain ? 2 : 1;

		if (blink)
		{
			if (c.is_pumping)
			{
				ch = 'P';
			}
			
			if (c.is_repairing)
			{
				ch = 'R';
			}
		}

		target->setChar(c.x + ox, c.y + oy, ch);
		target->setCharForeground(c.x + ox, c.y + oy, TCODColor(240, 240, 240));

		if (pos.cx - ox == c.x && pos.cy - oy == c.y)
		{
			target->setCharBackground(c.x + ox, c.y + oy, TCODColor(200, 200, 200));
			target->setCharForeground(c.x + ox, c.y + oy, TCODColor(128, 128, 128));

			if ((pos.lbutton_pressed && !(g_key.lctrl || g_key.rctrl)) && !in_context_menu)
			{
				selected = &crew[i];
				selected_crew = true;
			}
		}

		if (&crew[i] == selected)
		{
			target->setCharForeground(c.x + ox, c.y + oy, TCODColor(0, 0, 0));
			target->setCharBackground(c.x + ox, c.y + oy, TCODColor(255, 255, 255));
		}

		
	}

	if ((pos.lbutton_pressed && !(g_key.lctrl || g_key.rctrl)) && !selected_crew && !in_context_menu)
	{
		selected = nullptr;
	}

	if (!workbench_open)
	{
		for (int i = 0; i < workbenches.size(); i++)
		{
			bool highlight = false;

			for (auto v2 : workbenches[i]->tiles)
			{
				if (pos.cx - ox == v2.first && pos.cy - oy == v2.second)
				{
					highlight = true;
					break;
				}
			}

			if (highlight && !in_context_menu)
			{
				for (auto v2 : workbenches[i]->tiles)
				{
					target->setCharBackground(v2.first + ox, v2.second + oy, TCODColor(200, 200, 200));
				}

				if ((pos.lbutton_pressed && !(g_key.lctrl || g_key.rctrl)) && !selected_crew && !in_context_menu)
				{
					if (workbenches[i]->is_crewed())
					{
						workbenches[i]->open();
						workbench_open = workbenches[i];
						selected = nullptr;
					}
					else
					{
						// Play error sound TODO
					}
				}
			}
		}
	}

	// Context menu stuff
	do_context_menu(target, ox, oy, pos, torpedo_hlight);
	
}

void Vehicle::draw_window(TCODConsole* target)
{
	if (workbench_open != nullptr)
	{
		int x0 = (TCODConsole::root->getWidth() - workbench_open->get_size().first) / 2;
		int y0 = (TCODConsole::root->getHeight() - workbench_open->get_size().second) / 2;

		workbench_open->draw(x0, y0);


		Drawing::draw_window(TCODConsole::root, x0 - 1, y0 - 1, 
			workbench_open->get_size().first, workbench_open->get_size().second, workbench_open->get_name());

		TCODConsole::blit(workbench_open->get_console(), 0, 0,
			workbench_open->get_size().first - 1, workbench_open->get_size().second - 1, target, x0, y0);
	}
}

void Vehicle::move_order(Direction dir)
{
	if(maneouver->is_crewed())
	{
		std::string dir_name;
		if (dir == N)
		{
			dir_name = "North";
			maneouver->wanted_angle = 0.0f;
		}
		else if (dir == E) 
		{
			dir_name = "East";
			maneouver->wanted_angle = PI / 2.0f;
		}
		else if (dir == S)
		{
			dir_name = "South";
			maneouver->wanted_angle = PI;
		}
		else if(dir == W)
		{
			dir_name = "West";
			maneouver->wanted_angle = 3.0f * (PI / 2.0f);
		}
		else if (dir == NE)
		{
			dir_name = "North East";
			maneouver->wanted_angle = PI / 4.0f;
		}
		else if (dir == NW)
		{
			dir_name = "North West";
			maneouver->wanted_angle = 7.0f * (PI / 4.0f);
		}
		else if (dir == SE)
		{
			dir_name = "South East";
			maneouver->wanted_angle = 3.0f * (PI / 4.0f);
		}
		else
		{
			dir_name = "South West";
			maneouver->wanted_angle = 5.0f * (PI / 4.0f);
		}

		

		get_captain()->gc->speak("Go " + dir_name + "!");
	}
}

void Vehicle::speed_order(Speed speed)
{
	if (maneouver->is_crewed())
	{
		std::string name;
		if (speed == STOP)
		{
			name = "Stop!";
			maneouver->wanted_velocity = 0.0f;
		}
		else if (speed == SLOW)
		{
			name = "Move slow!";
			maneouver->wanted_velocity = 0.25f;
		}
		else if (speed == MEDIUM)
		{
			name = "Move normally!";
			maneouver->wanted_velocity = 0.5f;
		}
		else if (speed == FAST)
		{
			name = "Move fast!";
			maneouver->wanted_velocity = 0.75f;
		}
		else
		{
			name = "Full speed ahead!";
			maneouver->wanted_velocity = 1.0f;
		}

		get_captain()->gc->speak(name);
	}
}

void Vehicle::damage(float power, int x, int y)
{
	if (power < 1.0f)
	{
		power = 1.0f;
	}

	if (x == -1)
	{
		x = g_random->getInt(0, width);
	}

	if (y == -1)
	{
		y = g_random->getInt(0, height);
	}

	int poweri = (int)ceil(power);

	float xf = (float)x;
	float yf = (float)y;

	for (int x0 = x - poweri; x0 < x + poweri; x0++)
	{
		for (int y0 = y - poweri; y0 < y + poweri; y0++)
		{
			float x0f = (float)x0;
			float y0f = (float)y0;
			
			float dx = x0f - xf;
			float dy = y0f - yf;

			float dist = dx * dx + dy * dy;

			float chance = power / dist;

			if (g_random->getFloat(0.0f, 1.0f) <= chance)
			{
				if (x0 >= 0 && y0 >= 0 && x0 < width - 1 && y0 < height - 1)
				{
					tiles[y0 * width + x0].health -= 1.0f / sqrt(dist);
					if (tiles[y0 * width + x0].health < 0.0f)
					{
						tiles[y0 * width + x0].health = 0.0f;
					}
				}
			}
		}
	}
}

void Vehicle::electric_shock()
{
	if (battery_aux >= 0.25f / 3.0f) 
	{
		battery_aux -= 0.25f / 3.0f - 0.0001f;
		g_soloud->play(shock, 2.0f);

		ExplosionEffect fx = ExplosionEffect();
		fx.x = x;
		fx.y = y;
		fx.t = 0.3f;
		fx.dist = 1.0f;

		scene->expl_effects.push_back(fx);

		for (int i = 0; i < in_map->entities.size(); i++)
		{
			float dx = in_map->entities[i]->get_x() - x;
			float dy = in_map->entities[i]->get_y() - y;

			float dist = sqrt(dx * dx + dy * dy);

			if (dist < 0.35f)
			{
				in_map->entities[i]->damage(0.5f);
			}
		}
	}
}


Vehicle::Vehicle(FlightMap* map)
{
	water_tick = 0.25f;

	torpedo_0 = false;
	torpedo_1 = false;
	torpedo_2 = false;

	battery_a = 1.0f;
	battery_b = 1.0f;
	battery_aux = 0.25f;
	diesel = 1.0f;
	engines_on_diesel = false;

	blink = true;
	blinkt = 0.5f;

	selected = nullptr;
	breathing = false;
	workbench_open = nullptr;

	water_level = 0.0f;
	water_flow = 0.0f;
	noise = 0.0f;
	possible_speed = 0.0f;
		

	while (bubbles.size() <= 50)
	{
		float x = g_random->getFloat(0.0f, (float)WIDTH);
		float y = g_random->getFloat(0.0f, (float)40);
		Bubble b = Bubble();

		b.x = x; b.y = y;

		bubbles.push_back(b);
	}


	x = (float)map->width / 2.0f + 0.5f;
	y = (float)map->height / 2.0f + 0.5f;
	angle = 0.0f;
	velocity = 0.0f;
	flip_timer = 1.0f;

	engines_high.load("engines_high.wav"); engines_high.setLooping(true);
	engines_low.load("engines_low.wav"); engines_low.setLooping(true);
	moving_fast.load("moving_fast.wav"); moving_fast.setLooping(true);
	moving_slow.load("moving_slow.wav"); moving_slow.setLooping(true);
	waterflow.load("waterflow.wav"); waterflow.setLooping(true);

	engines_high_h = g_soloud->play(engines_high);
	engines_low_h = g_soloud->play(engines_low);
	moving_fast_h = g_soloud->play(moving_fast);
	moving_slow_h = g_soloud->play(moving_slow);
	waterflow_h = g_soloud->play(waterflow);

	shock.load("shock.wav");

	this->in_map = map;

	TCODConsole loaded_sub = TCODConsole(80, 60);
	loaded_sub.loadXp("Test.xp");
	
	// We have to "post process" the submarine into the map
	// Find lowest and highest tiles of the submarine
	
	int min_x = 10000, min_y = 10000, max_x = -10000, max_y = -10000;

	for (int x = 0; x < loaded_sub.getWidth(); x++)
	{
		for (int y = 0; y < loaded_sub.getHeight(); y++)
		{
			// Found tile
			if (!(loaded_sub.getCharBackground(x, y) == TCODColor(4, 24, 30) 
				&& (loaded_sub.getChar(x, y) == 0 || loaded_sub.getChar(x, y) == 32)))
			{
				if (x <= min_x)
				{
					min_x = x;
				}
				if (y <= min_y)
				{
					min_y = y;
				}
				if (x >= max_x)
				{
					max_x = x;
				}
				if (y >= max_y)
				{
					max_y = y;
				}
			}
		}
	}

	max_x++;
	max_y++;

	width = max_x - min_x;
	height = max_y - min_y;

	tiles.resize(width * height);

	for (int x = min_x; x < max_x; x++)
	{
		for (int y = min_y; y < max_y; y++)
		{
			int tx = x - min_x;
			int ty = y - min_y;

			VehicleTile* tile = &tiles[ty * width + tx];

			tile->ch = loaded_sub.getChar(x, y);
			tile->fore = loaded_sub.getCharForeground(x, y);
			tile->back = loaded_sub.getCharBackground(x, y);

			if (tile->ch == 238)
			{
				// Torpedo, replace with space and add a torpedo at location
				Torpedo tp = Torpedo();
				tp.x = tx; tp.y = ty;
				torpedoes.push_back(tp);

				tile->ch = ' ';
			}

			// Load crewmember in chairs and assign them the occupation
			if (tile->ch == 239)
			{
				FlightCrew c = FlightCrew(new Crewmember());
				c.x = tx; c.y = ty;
				c.gc->is_captain = tile->fore == TCODColor(222, 211, 195);
				c.cx = tx; c.cy = ty;

				Workbench* work = nullptr;


				if (c.gc->is_captain)
				{
					// Captain seat
				}
				else
				{
					// Find workbench
					if (loaded_sub.getChar(x, y - 1) == 30)
					{
						work = new Sonar(g_soloud);
						// Sonar
						work->tiles.push_back(std::make_pair(tx, ty - 1));
						work->tiles.push_back(std::make_pair(tx + 1, ty - 1));
						work->tiles.push_back(std::make_pair(tx - 1, ty - 1));
						
						sonar = (Sonar*)work;

					}
					else if (loaded_sub.getChar(x, y - 1) == 223)
					{
						// Maneouver
						work = new Maneouver();

						work->tiles.push_back(std::make_pair(tx, ty - 1));
						work->tiles.push_back(std::make_pair(tx + 1, ty - 1));
						work->tiles.push_back(std::make_pair(tx - 1, ty - 1));

						maneouver = (Maneouver*)work;
					}
					else if (loaded_sub.getChar(x, y + 1) == 220)
					{
						if (loaded_sub.getChar(x + 1, y + 1) == 63)
						{
							// Radio
							work = new Radio();

							work->tiles.push_back(std::make_pair(tx, ty + 1));
							work->tiles.push_back(std::make_pair(tx + 1, ty + 1));
							work->tiles.push_back(std::make_pair(tx - 1, ty + 1));

							radio = (Radio*)work;
						}
						else
						{
							work = new Targeting();

							// Listening station
							work->tiles.push_back(std::make_pair(tx, ty + 1));
							work->tiles.push_back(std::make_pair(tx + 1, ty + 1));
							work->tiles.push_back(std::make_pair(tx - 1, ty + 1));

							targeting = (Targeting*)work;
						}
					}
					else if (loaded_sub.getChar(x - 1, y) == 155)
					{
						work = new Periscope();

						// Periscope
						work->tiles.push_back(std::make_pair(tx - 1, ty));

						periscope = work;
					}
					else if (loaded_sub.getChar(x - 1, y) == 221)
					{
						work = new Machines();

						// Machinist
						work->tiles.push_back(std::make_pair(tx - 1, ty - 1));
						work->tiles.push_back(std::make_pair(tx - 1, ty + 0));
						work->tiles.push_back(std::make_pair(tx - 1, ty + 1));

						machines = (Machines*)work;
					}
					else if(loaded_sub.getChar(x, y + 1) == '{')
					{
						work = new Battery();

						// Battery specialist
						work->tiles.push_back(std::make_pair(tx, ty + 1));
						work->tiles.push_back(std::make_pair(tx + 1, ty + 1));
						work->tiles.push_back(std::make_pair(tx + 2, ty + 1));
						work->tiles.push_back(std::make_pair(tx + 3, ty + 1));
						work->tiles.push_back(std::make_pair(tx + 4, ty + 1));
						work->tiles.push_back(std::make_pair(tx + 5, ty + 1));
						work->tiles.push_back(std::make_pair(tx, ty + 1));

						work->tiles.push_back(std::make_pair(tx, ty - 7));
						work->tiles.push_back(std::make_pair(tx + 1, ty - 7));
						work->tiles.push_back(std::make_pair(tx + 2, ty - 7));
						work->tiles.push_back(std::make_pair(tx + 3, ty - 7));
						work->tiles.push_back(std::make_pair(tx + 4, ty - 7));
						work->tiles.push_back(std::make_pair(tx + 5, ty - 7));

						battery = work;
					}
					else
					{
						std::cout << "Malformed vehicle workbench at " << x << ", " << y << std::endl;
					}

					work->crew = &this->crew;
					work->set_vehicle(this);
					work->cx = tx;
					work->cy = ty;
					work->tiles.push_back(std::make_pair(work->cx, work->cy));

					workbenches.push_back(work);


					c.assigned.push_back(work);
				}

				crew.push_back(c);
			}

			if (tile->ch == 192)
			{
				m0x = tx; m0y = ty;
			}

			if (tile->ch == 218)
			{
				m1x = tx; m1y = ty;
			}

			tile->health = 1.0f;
			
			if (loaded_sub.getCharBackground(x, y) == TCODColor(4, 24, 30))
			{
				tile->is_outside = true;
				tile->blocks_player = true;
				tile->blocks_light = true;
				tile->blocks_water = false;
			}
			else
			{
				tile->is_outside = false;
				
				tile->blocks_player = false;
				tile->blocks_light = false;
				tile->blocks_water = false;

				// Load properties
				
				if ((tile->ch >= 179 && tile->ch <= 218) || tile->ch == '=')
				{
					// Walls (= -> Torpedo tube)
					tile->blocks_player = true;
					tile->blocks_light = true;
					tile->blocks_water = true;
				}

				if (tile->ch == '=')
				{
					tile->blocks_player = false;

					TorpedoSlot slot = TorpedoSlot();
					slot.x = tx;
					slot.y = ty;
					slot.has_torpedo = false;
					slot.crew_coming = -1;

					torpedo_slots.push_back(slot);
				}

				if (tile->ch == 'o')
				{
					// Airlock
					tile->blocks_player = false;
					tile->blocks_light = true;
					tile->blocks_water = true;
				}
			}
		}
	}

	tcod_map = new TCODMap(width, height);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			bool transparent = !tiles[y * width + x].blocks_light;
			bool walkable = !tiles[y * width + x].blocks_player;


			tcod_map->setProperties(x, y, transparent, walkable);
		}
	}

	dummy_tile.blocks_water = true;
}


Vehicle::~Vehicle()
{
	delete tcod_map;
}

