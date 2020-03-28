#include "EmbarkScene.h"
#include "../flight/Gamemaster.h"
#include "../flight/FlightScene.h"
#include "../flight/entity/Buildings.h"

void EmbarkScene::splatter(int x, int y, bool human)
{
	int bx = x + g_random->getInt(-2, 2);
	int by = y + g_random->getInt(-2, 2);

	if (bx > 0 && by > 0 && bx < map->width && by < map->height)
	{
		if (human)
		{
			map->tiles[by * map->width + bx].blood = true;
		}
		else
		{
			map->tiles[by * map->width + bx].alien_blood = true;
		}
		
	}
}

void EmbarkScene::update_entity(float dt, EmbarkMonster* monster)
{
	if (monster->health > 0.0f)
	{
		monster->walkt -= dt;
		monster->t0 -= dt;
		monster->t1 -= dt;
		monster->t2 -= dt;
		monster->attackt -= dt;

		if (monster->walkt <= 0.0f)
		{
			int nx, ny;
			if (monster->path->walk(&nx, &ny, false))
			{
				if (is_free_crew(nx, ny))
				{
					monster->x = nx;
					monster->y = ny;
				}
				else
				{
					// Attack crew at location
					if (monster->attackt < 0.0f)
					{
						for (EmbarkCrew& c : crew)
						{
							if (c.x == nx && c.y == ny && c.health >= 0.0f)
							{
								c.hurt(monster->damage, this);
								break;
							}
						}
						monster->attackt = monster->attack_interval;
					}
				}
			}

			monster->walkt = monster->walk_interval;
		}

		if (monster->t0 < 0.0f)
		{
			// Go towards players upon being seen
			if (monster->seen)
			{
				int tx, ty;
				int idx = g_random->getInt(0, crew.size() - 1);
				tx = crew[idx].x;
				ty = crew[idx].y;

				monster->path->compute(monster->x, monster->y, tx, ty);
			}

			if (monster->type == M_ARACHNID || monster->type == M_ROBOT)
			{
				monster->t0 = 3.0f;
			}
			else if (monster->type == M_GOLEM || monster->type == M_GRUNT)
			{
				monster->t0 = 6.0f;
			}
			else if (monster->type == M_ANCIENT)
			{
				monster->t0 = 10.0f;
			}
			else
			{
				monster->t0 = 5.0f;
			}
			
		}

	}
}

void EmbarkScene::spot(int x, int y, EmbarkCrew* crew)
{
	for (int i = 0; i < map->monsters.size(); i++)
	{
		if (map->monsters[i].x == x && map->monsters[i].y == y && !map->monsters[i].seen)
		{
			map->monsters[i].seen = true;
			switch (map->monsters[i].type)
			{
			case M_ARACHNID:
				crew->gc->speak("Spotted an arachnid.");
				break;
			case M_ANT:
				crew->gc->speak("Spotted a huge ant.");
				break;
			case M_GOLEM:
				crew->gc->speak("Spotted an alien golem.");
				break;
			case M_NEST:
				crew->gc->speak("Spotted the alien nest!");
				break;
			case M_ROBOT:
				if (!g_master->seen_robot)
				{
					crew->gc->speak("Wait! The hell is that robot doing here?");
					g_master->seen_robot = true;
				}
				else
				{
					crew->gc->speak("Spotted a robot.");
				}
				break;
			case M_GRUNT:
				if (!g_master->seen_grunt)
				{
					crew->gc->speak("Fuck! We haven't seen this one before.");
					g_master->seen_grunt = true;
				}
				else
				{
					crew->gc->speak("Spotted a humanoid.");

				}
				break;
			case M_ANCIENT:
				crew->gc->speak("Fucking hell, destroy that thing!");
				break;
			case M_COMPUTER:
				crew->gc->speak("Spotted a computer! It's their commander!");
				break;
			default:
				crew->gc->speak("Ayy lmao!");
				break;
			}
			
		}
	}
}

void EmbarkScene::handle_mouse()
{
	TCOD_mouse_t mouse = TCODMouse::getStatus();

	bool found = false;

	EmbarkCrew* n_select = nullptr;

	for (int i = 0; i < crew.size(); i++)
	{
		if (mouse.cx == crew[i].x && mouse.cy == crew[i].y)
		{
			// Draw info
			TCODConsole::root->printf(61, 61, crew[i].gc->name.c_str());
			TCODConsole::root->printf(62, 63, "Health: %i%%", (int)round(crew[i].health * 100.0f));
			TCODConsole::root->printf(62, 64, "Ammo: %i", crew[i].ammo);
			TCODConsole::root->printf(62, 65, "Grenades: %i", crew[i].grenades);

			if (crew[i].hold_fire)
			{
				TCODConsole::root->printf(62, 67, "Holding Fire", crew[i].ammo);
			}
			else
			{
				TCODConsole::root->printf(62, 67, "Firing automatically", crew[i].ammo);
			}

			n_select = &crew[i];

			found = true;
		}
	}

	if ((mouse.lbutton_pressed && !(g_key.lctrl || g_key.rctrl)))
	{
		selected = n_select;
	}

	if (selected && selected->health > 0.00001f)
	{
		bool was_set = false;

		TCODPath n_path = TCODPath(&map->tcod_map, 0.0f);


		// Draw the visibility map
		for (int x = 0; x < map->width; x++)
		{
			for (int y = 0; y < map->height; y++)
			{
				if (selected->view_map[y * map->width + x])
				{
					TCODConsole::root->setCharBackground(x, y, TCODColor(50, 30, 30));
				}

			}
		}

		if (is_free(mouse.cx, mouse.cy) && map->tiles[mouse.cy * map->width + mouse.cx].seen_before)
		{
			// Draw path
			n_path.compute(selected->x, selected->y, mouse.cx, mouse.cy);

			int sx = selected->x;
			int sy = selected->y;
			while (n_path.walk(&sx, &sy, false))
			{
				TCODConsole::root->setCharBackground(sx, sy, TCODColor::lightGrey);
			}

			if (mouse.mbutton_pressed || (mouse.lbutton_pressed && (g_key.lctrl || g_key.rctrl)))
			{
				selected->gc->speak("Moving!");
				was_set = true;
				selected->to_walk = new TCODPath(&map->tcod_map, 0.0f);
				selected->to_walk->compute(selected->x, selected->y, mouse.cx, mouse.cy);
			}
		}

		if (mouse.rbutton_pressed)
		{
			for (int i = 0; i < map->monsters.size(); i++)
			{
				if (map->monsters[i].x == mouse.cx && map->monsters[i].y == mouse.cy)
				{
					if (selected->view_map[mouse.cy * map->width + mouse.cx])
					{
						selected->target = &map->monsters[i];
						selected->gc->speak("Changed target!");
						break;
					}
				}
			}

		}

		if (mouse.rbutton_pressed && g_key.shift)
		{
			// Throw grenade
			if (selected->grenades > 0 && selected->view_map[mouse.cy * map->width + mouse.cx])
			{
				selected->gc->speak("Grenade out!");
				selected->grenades--;
				
				// Spawn grenade
				Grenade gnd = Grenade();
				gnd.x = mouse.cx; 
				gnd.y = mouse.cy;
				gnd.timer = 2.0f;
				gnd.exploded = false;

				g_soloud->play(grenade_land, 2.0f);

				grenades.push_back(gnd);
			}
		}
		
	}

	

	if (found)
	{
		TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
		TCODConsole::root->setCharForeground(mouse.cx, mouse.cy, TCODColor::black);
	}
}

void EmbarkScene::update_crew(float dt, EmbarkCrew* crew)
{
	if (crew->health > 0.00001f)
	{
		crew->walk_t -= dt;
		crew->fired_timer -= dt;
		if (crew->fired_timer < 0.0f)
		{
			crew->fired_timer = 0.0f;
		}

		if (crew->walk_t < 0.0f)
		{
			crew->walk_t = 0.4f;
			if (crew->to_walk != nullptr)
			{
				int nx, ny;
				if (!crew->to_walk->walk(&nx, &ny, true))
				{
					delete crew->to_walk;
					crew->to_walk = nullptr;
				}
				else
				{
					crew->x = nx;
					crew->y = ny;
				}
			}
		}

		if (crew->target == nullptr && !crew->hold_fire)
		{
			// Find a new target in field of view
			for (EmbarkMonster& monster : map->monsters)
			{
				if (crew->view_map[monster.y * map->width + monster.x] && monster.health > 0.0f)
				{
					crew->target = &monster;
					//crew->gc->speak("Acquired new target");
				}
			}
		}

		if (crew->target != nullptr)
		{
			if (crew->target->health <= 0.0f)
			{
				crew->target = nullptr;
			}
			else if (!crew->view_map[crew->target->y * map->width + crew->target->x])
			{
				//crew->gc->speak("Target lost");
				crew->target = nullptr;
			}
			else if (crew->fire(crew->target->x, crew->target->y, dt, &gunshot))
			{
				// Damage 
				if (crew->target->type == M_ROBOT || crew->target->type == M_GRUNT
					|| crew->target->type == M_ANCIENT || crew->target->type == M_COMPUTER)
				{
				}
				else
				{
					splatter(crew->target->x, crew->target->y, false);
				}

				crew->target->health -= 0.25f;
				if (crew->target->health <= 0.0f)
				{
					if (crew->target->type == M_ROBOT || crew->target->type == M_GRUNT
						|| crew->target->type == M_ANCIENT || crew->target->type == M_COMPUTER)
					{
						g_soloud->play(die_metal, 2.0f);
					}
					else
					{
						g_soloud->play(die_alien, 2.0f);
					}
				
					if (crew->target->type == M_NEST 
						|| crew->target->type == M_COMPUTER 
						|| crew->target->type == M_ANCIENT)
					{
						crew->gc->speak("Great job! Lets get out of here.");
					}
					else
					{
						crew->gc->speak("Target neutralized.");
					}

				}
				else
				{
					if (crew->target->type == M_ROBOT || crew->target->type == M_GRUNT
						|| crew->target->type == M_ANCIENT || crew->target->type == M_COMPUTER)
					{
						g_soloud->play(hurt_metal, 2.0f);
					}
					else
					{
						g_soloud->play(hurt_alien, 1.0f);
					}
					
				}
			}
		}
	}
	else
	{
		crew->fired_timer = 0.0f;
	}
}

void EmbarkScene::update(float dt)
{
	for (int i = 0; i < map->monsters.size(); i++)
	{
		if ((map->monsters[i].type == M_NEST ||
			map->monsters[i].type == M_COMPUTER ||
			map->monsters[i].type == M_ANCIENT) && map->monsters[i].health <= 0.0f)
		{
			if(map->monsters[i].type == M_COMPUTER && !mission_done)
			{
				g_popup->show("The computer had a databank inside which was uploaded automatically to the\n\
submarine. It will be scanned by the fatherland. Good job!");

				std::string coords = "";
				for (FlightEntity* ent : g_master->flight_scene->map.entities)
				{
					if (ent->get_type() == E_STATION && !((Building*)ent)->is_explored())
					{
						coords += std::to_string((int)floor(ent->get_x()));
						coords += ", ";
						coords += std::to_string((int)floor(ent->get_x()));
						break;
					}
				}

				g_master->flight_scene->vehicle.radio->push_message("We decoded the data-bank and found\
new coordinates: " + coords);
			}

			if (map->monsters[i].type == M_ANCIENT && !mission_done)
			{
				g_popup->show("Upon killing the huge robot, lights started flickering in all walls.\n\
Shortly after, a strong blast was heard. Something weird was going on outside, as the event was reported\
 by bases all over Jupiter-2. We had awoken something which was not meant to be perturbed.\
\n\n\n\n\
(To be continued, feel free to have fun exploring the world and fighting enemies!)");
			}

			mission_done = true;
		}
	}

	t += dt;

	/*if (g_key.vk == TCODK_F5)
	{
		finished = true;

		/*crew[0].health = 0.0f;
		crew[1].health = 0.0f;
		crew[2].health = 0.0f;
		crew[3].health = 0.0f;
	}*/

	if (!is_init)
	{
		if (!map->organic && g_master->clear_station_count == 3)
		{

			int it = 0;

			float max_dist = -100.0f;
			int max_x = 0;
			int max_y = 0;

			while (it < 1000)
			{
				int x = g_random->getInt(0, map->width - 1);
				int y = g_random->getInt(0, map->height - 1);
				float dx = x - map->spawn_x0;
				float dy = y - map->spawn_y0;
				float dist = sqrt(dx * dx + dy * dy);

				if (dist > max_dist && is_free(x, y))
				{
					max_dist = dist;
					max_x = x;
					max_y = y;
				}

				it++;
			}
			std::cout << "Spawn " << max_x << ", " << max_y << std::endl;

			EmbarkMonster m = EmbarkMonster(M_ANCIENT, &map->tcod_map);
			m.x = max_x;
			m.y = max_y;
			map->monsters.push_back(m);

			for (auto it = map->monsters.begin(); it != map->monsters.end(); it++)
			{
				if (it->type == M_COMPUTER)
				{
					map->monsters.erase(it);
					break;
				}
			}
		}

		// Position crew
		for (int i = 0; i < crew.size(); i++) 
		{
			int its = 0;
			int x = -1, y = -1;
			while (!is_free(x, y) && its < 100)
			{
				x = g_random->getInt(map->spawn_x0, map->spawn_x1);
				y = g_random->getInt(map->spawn_y0, map->spawn_y1);
				its++;
			}

			crew[i].x = x;
			crew[i].y = y;
			crew[i].view_map.resize(map->width * map->height);
		}

		is_init = true;
	}


	if (map == nullptr)
	{
		return;
	}

	blinkt -= dt;
	if (blinkt < 0.0f)
	{
		blink = !blink;
		blinkt = 0.5f;
	}

	for (int i = 0; i < crew.size(); i++) 
	{
		update_crew(dt, &crew[i]);
	}

	for (int i = 0; i < map->monsters.size(); i++) 
	{
		update_entity(dt, &map->monsters[i]);
	}

	for (int i = 0; i < grenades.size(); i++)
	{
		grenades[i].timer -= dt;
		if (grenades[i].timer <= 0.0f && !grenades[i].exploded)
		{
			// Damage around
			grenades[i].exploded = true;
			g_soloud->play(grenade, 3.0f);

			for (EmbarkMonster& mnst : map->monsters)
			{
				float dx = mnst.x - grenades[i].x;
				float dy = mnst.y - grenades[i].y;

				float dist = sqrt(dx * dx + dy * dy);

				if (dist < 3.0f)
				{
					mnst.health -= 1.0f;
				}
			}
		}
	}

}

void EmbarkScene::render()
{
	if (map == nullptr)
	{
		return;
	}

	for (int x = 0; x < map->width; x++)
	{
		for (int y = 0; y < map->height; y++)
		{
			map->tiles[y * map->width + x].seen_by_someone = false;
		}
	}

	// Compute visibility
	for (int i = 0; i < crew.size(); i++)
	{
		map->tcod_map.computeFov(crew[i].x, crew[i].y, 18, true, FOV_DIAMOND);

		for (int x = 0; x < map->width; x++)
		{
			for (int y = 0; y < map->height; y++)
			{
				if (map->tcod_map.isInFov(x, y))
				{
					if (!map->tiles[y * map->width + x].seen_before)
					{
						spot(x, y, &crew[i]);
					}

					map->tiles[y * map->width + x].seen_by_someone = true;
					map->tiles[y * map->width + x].seen_before = true;
					crew[i].view_map[y * map->width + x] = true;
				}
				else
				{
					crew[i].view_map[y * map->width + x] = false;
				}
			}
		}
	}

	for (int x = 0; x < map->width; x++)
	{
		for (int y = 0; y < map->height; y++)
		{
			if (map->tiles[y * map->width + x].seen_by_someone)
			{
				map->tiles[y * map->width + x].seen = true;
			}
			else
			{
				map->tiles[y * map->width + x].seen = false;
			}
		}
	}

	map->draw(*TCODConsole::root);

	// Check for all alive crew to be in square
	int alive = 0;
	int alive_in_square = 0;
	for (EmbarkCrew& c : crew)
	{
		if (c.health > 0.0001f)
		{
			alive++;

			if (c.x >= map->spawn_x0 && c.y >= map->spawn_y0 && c.x <= map->spawn_x1 && c.y <= map->spawn_y1)
			{
				alive_in_square++;
			}
		}
	}

	if (alive == 0)
	{
		finished = true;
	}


	if (mission_done)
	{
		float gf = (sin(t) + 1.0f) * 0.5f;
		int g = (int)round(gf * 255);
		TCODColor col = TCODColor(0, g, 0);
		for (int x = map->spawn_x0; x <= map->spawn_x1; x++)
		{
			for (int y = map->spawn_y0; y <= map->spawn_y1; y++)
			{
				TCODConsole::root->setCharBackground(x, y, col);
			}
		}


		if (alive_in_square == alive)
		{
			finished = true;
		}
		else
		{
			finished = false;
		}
	}


	handle_mouse();

	// Draw crew
	for (int i = 0; i < crew.size(); i++)
	{
		if (&crew[i] == selected && blink)
		{
			TCODConsole::root->setChar(crew[i].x, crew[i].y, 'X');
		}
		else
		{
			TCODConsole::root->setChar(crew[i].x, crew[i].y, crew[i].gc->is_captain ? 2 : 1);
		}

		if (crew[i].fired_timer > 0.0f)
		{
			//TCODConsole::root->setCharForeground(crew[i].x, crew[i].y, TCODColor::yellow);
			// Draw bullet path in background color
			TCODLine line = TCODLine();
			line.init(crew[i].x, crew[i].y, crew[i].fx, crew[i].fy);
			int x, y;
			while (!line.step(&x, &y))
			{
				TCODConsole::root->setCharBackground(x, y, TCODColor::lightYellow);
			}
		}
		

		TCODConsole::root->setCharForeground(crew[i].x, crew[i].y, TCODColor::white);

		if (crew[i].health <= 0.4f && crew[i].health > 0.0f && !blink)
		{
			TCODConsole::root->setCharForeground(crew[i].x, crew[i].y, TCODColor::darkRed);
		}

		if (crew[i].health <= 0.0f) 
		{
			TCODConsole::root->setCharForeground(crew[i].x, crew[i].y, TCODColor::red);
		}
	
		
		

	}

	// Draw grenades
	for (int i = 0; i < grenades.size(); i++)
	{
		if (grenades[i].timer <= 1.0f && !grenades[i].exploded)
		{
			TCODConsole::root->setChar(grenades[i].x, grenades[i].y, 147);
		}
		
		if (grenades[i].timer >= -1.0f && grenades[i].exploded)
		{
			float itim = 1.0f + grenades[i].timer;
			// Draw explosion circle

			for (int x = 0; x < map->width; x++)
			{
				for (int y = 0; y < map->height; y++)
				{
					float dx = x - grenades[i].x;
					float dy = y - grenades[i].y;

					float dist = sqrt(dx * dx + dy * dy);

					if (dist < itim * 10.0f)
					{
						TCODConsole::root->setCharBackground(x, y, TCODColor::lightGrey);
						TCODConsole::root->setChar(x, y, '+');
						TCODConsole::root->setCharForeground(x, y, TCODColor::white);
					}
				}
			}
		}
	}

	// Draw the status box
	for (int x = 0; x < WIDTH; x++)
	{
		TCODConsole::root->setChar(x, 60, 205);
		TCODConsole::root->setCharForeground(x, 60, TCODColor::lightGrey);
	}

	g_status->draw(0, 60, WIDTH, HEIGHT);

	// Draw the info box

	for (int y = 60; y < HEIGHT; y++)
	{
		TCODConsole::root->setChar(60, y, 186);
		TCODConsole::root->setCharForeground(60, y, TCODColor::lightGrey);
	}

	TCODConsole::root->setChar(60, 60, 203);
	TCODConsole::root->setCharForeground(60, 60, TCODColor::lightGrey);



}

bool EmbarkScene::is_free(int x, int y, int ignore_ent)
{
	if (x < 0 || y < 0 || x >= map->width || y >= map->height)
	{
		return false;
	}

	if (map->tiles[y * map->width + x].is_wall)
	{
		return false;
	}

	for (int i = 0; i < crew.size(); i++) 
	{
		if (crew[i].x == x && crew[i].y == y && crew[i].health > 0.00001f)
		{
			return false;
		}
	}

	for (int i = 0; i < map->monsters.size(); i++)
	{
		if (map->monsters[i].x == x && map->monsters[i].y == y && i != ignore_ent
			&& map->monsters[i].health > 0.00001f) 
		{
			return false;
		}
	}

	return true;
}

bool EmbarkScene::is_free_crew(int x, int y)
{
	if (x < 0 || y < 0 || x >= map->width || y >= map->height)
	{
		return false;
	}

	if (map->tiles[y * map->width + x].is_wall)
	{
		return false;
	}

	for (int i = 0; i < crew.size(); i++)
	{
		if (crew[i].x == x && crew[i].y == y && crew[i].health > 0.00001f)
		{
			return false;
		}
	}
}

void EmbarkScene::restart()
{
	crew.clear();
	grenades.clear();
	selected = nullptr;
	finished = false;
	t = 0.0f;
	mission_done = false;
	is_init = false;
	map = nullptr;
}

EmbarkScene::EmbarkScene()
{
	finished = false;
	mission_done = false;

	map = nullptr;
	is_init = false;
	blinkt = 0.0f;

	gunshot.load("gunshot.wav");
	grenade.load("grenade.wav");
	grenade_land.load("grenade_land.wav");
	hurt.load("hurt.wav");
	hurt_alien.load("hurt_alien.wav");
	hurt_metal.load("hurt_metal.wav");

	die_alien.load("alien_die.wav");
	die_man.load("man_die.wav");
	die_metal.load("metal_die.wav");
}


EmbarkScene::~EmbarkScene()
{
}

EmbarkMonster::EmbarkMonster(MonsterType type, TCODMap* map)
{
	path = new TCODPath(map, 0.0f);

	this->seen = false;
	this->type = type;
	this->walkt = 0.0f;
	t0 = 0.0f;
	t1 = 0.0f;
	t2 = 0.0f;

	switch (type)
	{
	case M_ARACHNID:
		health = 0.5f;
		walk_interval = 0.05f;
		damage = 0.1f;
		attack_interval = 0.8f;
		break;
	case M_ANT:
		health = 0.8f;
		walk_interval = 0.15f;
		damage = 0.25f;
		attack_interval = 0.8f;
		break;
	case M_GOLEM:
		health = 5.0f;
		walk_interval = 0.6f;
		damage = 0.3f;
		attack_interval = 2.0f;
		break;
	case M_NEST:
		health = 8.0f;
		walk_interval = 10000000000.0f;
		damage = 0.0f;
		break;
	case M_ROBOT:
		health = 1.5f;
		walk_interval = 0.25f;
		damage = 0.1f;
		attack_interval = 0.5f;
		break;
	case M_GRUNT:
		health = 3.0f;
		walk_interval = 0.5f;
		damage = 0.5f;
		attack_interval = 1.5f;
		break;
	case M_ANCIENT:
		health = 11.0f;
		walk_interval = 0.7f;
		damage = 0.8f;
		attack_interval = 1.0f;
		break;
	case M_COMPUTER:
		health = 8.0f;
		walk_interval = 10000000000.0f;
		damage = 0.0f;
		break;
	}
}

int EmbarkMonster::get_symbol()
{
	switch (type)
	{
	case M_ARACHNID:
		return 'x';
	case M_ANT:
		return 12;
	case M_GOLEM:
		return 227;
	case M_NEST:
		return 15;
	case M_ROBOT:
		return 232;
	case M_GRUNT:
		return 228;
	case M_ANCIENT:
		return 234;
	case M_COMPUTER:
		return 241;
	}
}

TCODColor EmbarkMonster::get_foreground()
{
	if (type == M_ROBOT || type == M_GRUNT || type == M_COMPUTER)
	{
		return TCODColor(102, 155, 255);
	}
	else if (type == M_ANCIENT)
	{
		return TCODColor(205, 133, 220);
	}
	else
	{
		return TCODColor(140, 102, 255);
	}

}

void EmbarkCrew::hurt(float dmg, EmbarkScene* scene)
{
	// Attack
	health -= dmg;
	if (health < 0.0f) 
	{
		health = 0.0f;

		scene->splatter(x, y, true);
		scene->splatter(x, y, true);
		g_soloud->play(scene->die_man, 2.0f);
	}
	else
	{
		scene->splatter(x, y, true);
		g_soloud->play(scene->hurt, 2.0f);
	}

	if (g_random->getFloat(0.0f, 1.0f) > 0.5f)
	{
		gc->speak("I'm Hurt!");
	}
}

bool EmbarkCrew::fire(int tx, int ty, float dt, SoLoud::Wav * sound)
{
	float dx = tx - x;
	float dy = ty - y;
	float dist = sqrt(dx * dx + dy * dy);

	float fail_chance = 0.6f;

	if (dist <= 1.0f) 
	{
		fail_chance = 0.1f;
	}
	else if (dist <= 3.0f)
	{
		fail_chance = 0.3f;
	}
	

	int rx = tx; int ry = ty;
	if (g_random->getFloat(0.0f, 1.0f) < fail_chance)
	{
		// Miss (may actually not miss, but very low prob)
		rx += g_random->getInt(-2, 2);
		ry += g_random->getInt(-2, 2);
	}


	fire_timer -= dt;
	if (fire_timer < 0.0f) 
	{
		g_soloud->play(*sound, 1.0f);

		TCODConsole::root->setCharForeground(x, y, TCODColor::yellow);

		// Do the actual shot

		fire_timer = g_random->getFloat(1.0f, 3.0f);
		fired_timer = g_random->getFloat(0.3f, 0.5f);
		fx = rx;
		fy = ry;

		if (rx == tx && ry == ty)
		{
			return true;
		}
	}

	return false;
}
