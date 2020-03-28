#include "Monsters.h"
#include "../FlightScene.h"
#include "../FlightMap.h"



void EntityCrawler::update(float dt)
{
	Monster::update(dt);
	
	if (c_x == -1)
	{
		c_x = (int)floor(get_x());
		c_y = (int)floor(get_x());
	}

	speed_penalty -= dt * get_speed_penalty_rate();
	if (speed_penalty < 0.0f)
	{
		speed_penalty = 0.0f;
	}

	if (persecuting)
	{
		c_x = (int)floor(get_x());
		c_y = (int)floor(get_x());

		auto player = get_player_pos();
		go_to(player.first, player.second);

		velocity = max(0.25f - speed_penalty * 10.0f, 0.0f);

		if (distance_to_player() <= 0.05f && speed_penalty == 0.0f)
		{
			// Attack and slowdown
			g_soloud->play(attack, 2.0f);

			do_damage();
			
			speed_penalty = 1.0f;
		}
	}
	else
	{
		velocity = 0.1f;

		// Move idly, if we hear the player we have a chance to attack
		if (in_destination())
		{
			idle(5, c_x, c_y);
		}

		if (distance_to_player() <= 1.0f + get_player_noise() * 4.0f && g_random->getFloat(0.0f, 1.0f) >= 0.98f
			|| autofind())
		{
			persecuting = true;
			g_soloud->play(persecute, 0.6f);
		}
	}
}

void EntityCrawler::do_damage()
{
	if (g_random->getFloat(0.0f, 1.0f) >= 0.6f)
	{
		// Damage
		g_soloud->play(impact, 2.0f);
		get_player()->damage(g_random->getFloat(1.3f, 2.0f));
	}
}


void EntityWorm::update(float dt)
{
	Monster::update(dt);

	auto player = get_player_pos();

	if (dive)
	{
		velocity = 0.5f;
		go_to(player.first, player.second);

		if (distance_to_player() <= 0.15f)
		{
			dive = false;
			g_soloud->play(attack_sound, 2.0f);
			get_player()->damage(g_random->getFloat(1.8f, 3.2f));

		}
	}
	else
	{
		if (persecuting)
		{
			// Worms are free to go through walls!
			rand_timer -= dt;
			if (rand_timer <= 0.0f)
			{
				float offset_x = g_random->getFloat(-1.8f, 1.8f);
				float offset_y = g_random->getFloat(-1.8f, 1.8f);
				move_to(player.first + offset_x, player.second + offset_y);
				rand_timer = 3.0f;
			}
			

		
			
			velocity = 0.1f;

			if (distance_to_player() <= 1.3f && distance_to_player() >= 0.6f 
				&& g_random->getFloat(0.0f, 1.0f) >= 0.994f)
			{
				dive = true;
				g_soloud->play(dive_sound, 2.0f);
			}
		}
		else
		{
			velocity = 0.1f;

			// Move idly, if we hear the player we have a chance to attack
			if (in_destination())
			{
				idle(5, (int)get_x(), (int)get_y());
			}

			if (distance_to_player() <= 1.0f + get_player_noise() * 7.0f && g_random->getFloat(0.0f, 1.0f) >= 0.95f)
			{
				persecuting = true;
				//g_soloud->play(persecute, 0.6f);
			}
		}
	}
}



void Monster::path_to(int x, int y)
{
	if(x < 0){ x = 0; }
	if(y < 0){ y = 0; }
	if(x >= get_map()->width){ x = get_map()->width - 1; }	
	if(y >= get_map()->height){ y = get_map()->height - 1; }
	
	if (current_path != nullptr)
	{
		int dx, dy;
		current_path->getDestination(&dx, &dy);

		if (dx == x && dy == y)
		{
			return;
		}
	}

	TCODPath* n_path = new TCODPath(&get_map()->vmap, 0.0f);

	auto t = get_map_position();
	
	if (n_path->compute(t.first, t.second, x, y))
	{
		current_path = n_path;
		just_started = true;
	}
	else
	{
		x_target = -1.0f;
		y_target = -1.0f;
		delete n_path;
	}


}


void Monster::update(float dt)
{
	// Update basic movement
	if (!in_destination())
	{
		float x = get_x();
		float y = get_y();

		float dx = x_target - x;
		float dy = y_target - y;
		float l = sqrt(dx * dx + dy * dy);
		float xn = dx / l;
		float yn = dy / l;
		
		if (l <= get_speed() * 0.01f)
		{
			set_position(x_target, y_target);
		}
		else
		{
			set_position(x + xn * get_speed() * dt, y + yn * get_speed() * dt);
		}
	}


	if (current_path != nullptr)
	{
		auto t = get_map_position();

		if (just_started)
		{
			if (std::abs(get_x() - t.first - 0.5f) <= 0.2f &&
				std::abs(get_y() - t.second - 0.5f) <= 0.2f)
			{
				// We are sufficiently close to the center of the tile
				just_started = false;
				step();
			}
			else
			{
				// Move to center of tile
				move_to((float)t.first + 0.5f, (float)t.second + 0.5f);

				if (in_destination())
				{
					just_started = false;
					step();
				}
			}
		}
		else
		{
			// Move to center of next tile
			if (in_destination())
			{
				step();
			}
		}

	}
}

std::pair<int, int> Monster::get_map_position()
{
	std::pair<float, float> pos = get_position();

	return std::make_pair((int)floor(pos.first), (int)floor(pos.second));
}

void Monster::step()
{
	if (current_path != nullptr)
	{
		int nx, ny;
		if (!current_path->walk(&nx, &ny, true))
		{
			delete current_path;
			current_path = nullptr;
			return;
		}

		move_to((float)nx + 0.5f, (float)ny + 0.5f);
	}
}

void Monster::move_to(float x, float y)
{
	x_target = x;
	y_target = y;
}

bool Monster::in_destination()
{
	if (x_target == -1.0f)
	{
		return true;
	}
	else
	{
		return x_target == get_x() && y_target == get_y();
	}
}

float Monster::go_to(float px, float py)
{
	float x = get_x();
	float y = get_y();

	if (floor(px) == floor(x) && floor(py) == floor(y))
	{
		// Same tile, simple path
		move_to(px, py);
	}
	else
	{
		// Other tile, path
		path_to((int)floor(px), (int)floor(py));
	}

	float dx = px - x;
	float dy = py - y;
	return sqrt(dx * dx + dy * dy);
}

void Monster::idle(int radius, int cx, int cy)
{
	int t_x = g_random->getInt(cx - radius, cx + radius);
	int t_y = g_random->getInt(cy - radius, cy + radius);

	path_to(t_x, t_y);
}

float Monster::distance_to(float x, float y)
{
	float dx = get_x() - x;
	float dy = get_y() - y;
	return sqrt(dx * dx + dy * dy);
}

float Monster::distance_to_player()
{
	float px = get_scene()->vehicle.x;
	float py = get_scene()->vehicle.y;
	return distance_to(px, py);
}

std::pair<float, float> Monster::get_player_pos()
{
	return std::make_pair(get_scene()->vehicle.x, get_scene()->vehicle.y);
}

float Monster::get_player_noise()
{
	return get_scene()->vehicle.noise;
}

Vehicle* Monster::get_player()
{
	return &get_scene()->vehicle;
}

void EntityBiter::do_damage()
{
	if (g_random->getFloat(0.0f, 1.0f) >= 0.3f)
	{
		// Damage
		g_soloud->play(impact, 1.6f);
		get_player()->damage(1.2f);
	}
}

void EntityBomb::do_damage()
{
	damage(1.0f);
	get_scene()->explosion(get_x(), get_y(), 0.1f);
}

void EntityGuardian::update(float dt)
{
	Monster::update(dt);

	velocity = 0.02f;

	if (c_x == -1.0f)
	{
		c_x = get_x();
		c_y = get_y();
	}

	float r = 3.0f;

	move_timer -= dt;
	if (move_timer <= 0.0f)
	{
		float ox = g_random->getFloat(-r, r);
		float oy = g_random->getFloat(-r, r);
		move_to(c_x + ox, c_y + oy);
		move_timer = 10.0f;
	}

	// Difference to base
	float dxb = get_player()->x - c_x;
	float dyb = get_player()->y - c_y;

	float base_dist = sqrt(dxb * dxb + dyb * dyb);

	if (base_dist <= r)
	{
		// Send hell
		spawn_timer -= dt;

		if (spawn_timer <= 0.0f)
		{
			FlightEntity* ent;
			if (g_random->getFloat(0.0f, 1.0f) >= 0.45f)
			{
				// Biter
				ent = new EntityBiter();
			}
			else
			{
				// Bomb
				ent = new EntityBomb();
			}

			ent->set_position(get_x(), get_y());
			ent->init(get_map(), get_scene());
			get_map()->entities.push_back(ent);

			spawn_timer = g_random->getFloat(10.0f, 35.0f);
		}
	}
}

void EntitySerpent::update(float dt)
{
	Monster::update(dt);

	engage_timer -= dt;

	if (engaging)
	{
		velocity = 0.25f;

		fight_timer -= dt;
		if (fight_timer < 0.0f)
		{
			float px = get_player()->x;
			float py = get_player()->y;
			float ox = g_random->getFloat(-1.7f, 1.7f);
			float oy = g_random->getFloat(-1.7f, 1.7f);

			if (std::abs(ox) < 0.4f)
			{
				ox = ox > 0.0f ? 0.35f : -0.4f;
			}

			if (std::abs(oy) < 0.4f)
			{
				oy = oy > 0.0f ? 0.4f : -0.4f;
			}

			move_to(px + ox, py + oy);
			fight_timer = 4.0f;
		}

		spawn_timer -= dt;
		if (spawn_timer < 0.0f)
		{
			FlightEntity* ent;
			if (g_random->getFloat(0.0f, 1.0f) >= 0.5f)
			{
				if (g_random->getFloat(0.0f, 1.0f) >= 0.5f)
				{
					// Biter
					ent = new EntityBiter();
				}
				else
				{
					// Bomb
					ent = new EntityBomb();
				}

			}
			else
			{
				// Crawler
				ent = new EntityCrawler();
			}

			ent->set_position(get_x(), get_y());
			ent->init(get_map(), get_scene());
			get_map()->entities.push_back(ent);

			spawn_timer = g_random->getFloat(5.0f, 30.0f);
		}

		if (engage_timer < 0.0f)
		{
			engaging = false;
		}
	}
	else
	{
		velocity = 0.15f;

		// Move idly, if we hear the player we have a chance to attack
		if (in_destination())
		{
			idle(5, (int)get_x(), (int)get_y());
		}

		if (engage_timer <= -60.0f)
		{
			if (distance_to_player() <= 1.0f + get_player_noise() * 7.0f && g_random->getFloat(0.0f, 1.0f) >= 0.95f)
			{
				engaging = true;
				//g_soloud->play(persecute, 0.6f);
				engage_timer = 60.0f;
			}
		}

	}
}
