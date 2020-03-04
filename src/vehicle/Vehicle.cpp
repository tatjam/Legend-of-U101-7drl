#include "Vehicle.h"



void Vehicle::update(float dt)
{
	for (size_t i = 0; i < workbenches.size(); i++)
	{
		if (workbenches[i]->update(dt))
		{
			workbenches[i]->close();
			workbench_open = false;
		}
	}

	auto old = get_tile();
	bool was_breathing = breathing;
	
	x += sin(angle) * velocity * dt * 0.15f;
	y -= cos(angle) * velocity * dt * 0.15f;

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
					maneouver->get_crewman()->speak("We have entered an oxygen rich zone.");
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
					maneouver->get_crewman()->speak("We have exited the oxygen rich zone.");
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

	blinkt -= dt;
	if (blinkt <= 0.0f)
	{
		blinkt = 0.5f;
		blink = !blink;
	}

	for (int i = 0; i < crew.size(); i++) 
	{
		crew[i].path_t -= dt;

		if (crew[i].path != nullptr && crew[i].path_t <= 0.0f)
		{
			int nx, ny;
			crew[i].path->walk(&crew[i].x, &crew[i].y, true);

			crew[i].path_t = 0.1f;
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
				target->setChar(x + ox, y + oy, tiles[y * width + x].ch);
				target->setCharForeground(x + ox, y + oy, tiles[y * width + x].fore);
				target->setCharBackground(x + ox, y + oy, tiles[y * width + x].back);

			}
		}
	}

	// Draw torpedoes
	for (int i = 0; i < torpedoes.size(); i++)
	{
		Torpedo& tp = torpedoes[i];
		target->setChar(tp.x + ox, tp.y + oy, 238);
		target->setChar(tp.x + 1 + ox, tp.y + oy, TCOD_CHAR_DHLINE);
		target->setChar(tp.x + 2 + ox, tp.y + oy, TCOD_CHAR_DHLINE);
		target->setChar(tp.x + 3 + ox, tp.y + oy, TCOD_CHAR_DHLINE);
		target->setChar(tp.x + 4 + ox, tp.y + oy, 232);
		target->setCharForeground(tp.x + ox, tp.y + oy, TCODColor(140, 70, 0));
		target->setCharForeground(tp.x + 1 + ox, tp.y + oy, TCODColor(158, 158, 158));
		target->setCharForeground(tp.x + 2 + ox, tp.y + oy, TCODColor(158, 158, 158));
		target->setCharForeground(tp.x + 3 + ox, tp.y + oy, TCODColor(158, 158, 158));
		target->setCharForeground(tp.x + 4 + ox, tp.y + oy, TCODColor(100, 100, 100));
	}

	// Draw crew
	bool selected_crew = false;
	for (int i = 0; i < crew.size(); i++)
	{
		Crewmember& c = crew[i];
		
		target->setChar(c.x + ox, c.y + oy, c.is_captain ? 2 : 1);
		target->setCharForeground(c.x + ox, c.y + oy, TCODColor(240, 240, 240));

		if (pos.cx - ox == c.x && pos.cy - oy == c.y)
		{
			target->setCharBackground(c.x + ox, c.y + oy, TCODColor(200, 200, 200));
			target->setCharForeground(c.x + ox, c.y + oy, TCODColor(128, 128, 128));

			if (pos.lbutton_pressed && !in_context_menu)
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

	if (pos.lbutton_pressed && !selected_crew && !in_context_menu)
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

				if (pos.lbutton_pressed && !selected_crew && !in_context_menu)
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
		if (pos.mbutton_pressed)
		{
			selected->path_to(pos.cx - ox, pos.cy - oy, *tcod_map);
		}
	}

	if (in_context_menu)
	{
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
		}

		menu_items.push_back("View Crewmember");



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

			if (highlight && pos.lbutton_pressed)
			{
				if (menu_items[i] == "Move Here")
				{
					selected->path_to(ctx_x, ctx_y, *tcod_map);
				}
			}
			
		}

		if (pos.lbutton_pressed)
		{
			in_context_menu = false;
		}

		target->setAlignment(TCOD_LEFT);
	}
	
}

void Vehicle::draw_window(TCODConsole* target)
{
	if (workbench_open)
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

		

		maneouver->get_crewman()->speak(Speech::positive() + Speech::heading() + dir_name);
	}
}

void Vehicle::speed_order(Speed speed)
{
	if (maneouver->is_crewed())
	{
		std::string name;
		if (speed == STOP)
		{
			name = "Stopping!";
			maneouver->wanted_velocity = 0.0f;
		}
		else if (speed == SLOW)
		{
			name = "Moving slowly!";
			maneouver->wanted_velocity = 0.25f;
		}
		else if (speed == MEDIUM)
		{
			name = "Moving normally!";
			maneouver->wanted_velocity = 0.5f;
		}
		else if (speed == FAST)
		{
			name = "Moving fast!";
			maneouver->wanted_velocity = 0.75f;
		}
		else
		{
			name = "Full speed ahead!";
			maneouver->wanted_velocity = 1.0f;
		}

		maneouver->get_crewman()->speak(name);
	}
}


Vehicle::Vehicle(FlightMap* map)
{


	blink = true;
	blinkt = 0.5f;

	selected = nullptr;
	breathing = false;

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

	engines_high_h = g_soloud->play(engines_high);
	engines_low_h = g_soloud->play(engines_low);
	moving_fast_h = g_soloud->play(moving_fast);
	moving_slow_h = g_soloud->play(moving_slow);

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
				Crewmember c = Crewmember();
				c.x = tx; c.y = ty;
				c.is_captain = tile->fore == TCODColor(222, 211, 195);

				Workbench* work = nullptr;


				if (c.is_captain)
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
							work = new Listening();

							// Listening station
							work->tiles.push_back(std::make_pair(tx, ty + 1));
							work->tiles.push_back(std::make_pair(tx + 1, ty + 1));
							work->tiles.push_back(std::make_pair(tx - 1, ty + 1));

							listening = work;
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

						machines = work;
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
					tile->health = 100.0f;
				}

				if (tile->ch == 'o')
				{
					// Airlock
					tile->blocks_player = false;
					tile->blocks_light = true;
					tile->blocks_water = true;
					tile->health = 60.0f;
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
}


Vehicle::~Vehicle()
{
	delete tcod_map;
}

