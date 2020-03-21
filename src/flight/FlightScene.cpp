#include "FlightScene.h"
#include "entity/Buildings.h"



void FlightScene::update(float dt)
{
	underwater.setVolume(0.6f);

	t += dt;

	for (int i = 0; i < map.entities.size(); i++)
	{
		if (map.entities[i]->is_alive())
		{
			map.entities[i]->update(dt);
		}
	}

	for (auto it = expl_effects.begin(); it != expl_effects.end();)
	{
		it->t -= dt;

		if (it->t < 0.0f)
		{
			it = expl_effects.erase(it);
		}
		else
		{
			it++;
		}
	}


	vehicle.update(dt);

	if (!sent_start_message && t >= 6.0f) 
	{
		std::string str = "\
Command to U101.\n\
Prepare your vehicle for further orders from the fatherland.\n\
We are now going to send the location of all our outposts.\n";

		for (int i = 0; i < map.lbases.size(); i++) 
		{
			auto t = map.lbases[i];
			str += "O" + std::to_string(i + 1) + ": " + std::to_string(t.first) + ", " + std::to_string(t.second) + ".\n";
		}

		str += "\nYou are currently on the outpost O";
		str += std::to_string(map.lbases.size());

		vehicle.radio->push_message(str);
		sent_start_message = true;
	}

	if (g_key.vk == TCODK_KP8 || g_key.c == 'k')
	{
		vehicle.move_order(N);
	}

	if (g_key.vk == TCODK_KP2 || g_key.c == 'j')
	{
		vehicle.move_order(S);
	}

	if (g_key.vk == TCODK_KP6 || g_key.c == 'l')
	{
		vehicle.move_order(E);
	}

	if (g_key.vk == TCODK_KP4 || g_key.c == 'h')
	{
		vehicle.move_order(W);
	}

	if (g_key.vk == TCODK_KP1 || g_key.c == 'b')
	{
		vehicle.move_order(SW);
	}

	if (g_key.vk == TCODK_KP7 || g_key.c == 'y')
	{
		vehicle.move_order(NW);
	}

	if (g_key.vk == TCODK_KP9 || g_key.c == 'u')
	{
		vehicle.move_order(NE);
	}

	if (g_key.vk == TCODK_KP3 || g_key.c == 'n')
	{
		vehicle.move_order(SE);
	}

	if (g_key.c == 'r')
	{
		for (int i = 0; i < vehicle.crew.size(); i++)
		{
			vehicle.crew[i].path_to(vehicle.crew[i].cx, vehicle.crew[i].cy, *vehicle.tcod_map);

			if (vehicle.crew[i].gc->is_captain)
			{
				vehicle.crew[i].gc->speak("Everybody return to positions!");
			}
		}
	}

	if (g_key.vk == TCODK_1)
	{
		vehicle.speed_order(STOP);
	}

	if (g_key.vk == TCODK_2)
	{
		vehicle.speed_order(SLOW);
	}

	if (g_key.vk == TCODK_3)
	{
		vehicle.speed_order(MEDIUM);
	}

	if (g_key.vk == TCODK_4)
	{
		vehicle.speed_order(FAST);
	}

	if (g_key.vk == TCODK_5)
	{
		vehicle.speed_order(FULL);
	}


	if (g_key.c == 'f')
	{
		vehicle.targeting->fire_torpedo();
	}

	if (g_key.c == 'e')
	{
		// Electric shock
		vehicle.electric_shock();
	}

	for (FlightEntity* ent : map.entities)
	{
		possible_embark = nullptr;

		if (ent->get_type() == E_NEST || ent->get_type() == E_STATION)
		{
			float dx = vehicle.x - ent->get_x();
			float dy = vehicle.y - ent->get_y();
			float dist = sqrt(dx * dx + dy * dy);

			bool is_explored = ((Building*)ent)->is_explored();

			if (dist < 0.2f && !is_explored)
			{
				possible_embark = ent;
				break;
			}
		}
		/*if (ent->get_type() == E_NEST && !((Building*)ent)->is_explored())
		{		
			possible_embark = ent;
			break;
		}*/
	}

	if (possible_embark != nullptr)
	{
		if (g_key.c == 'g')
		{
			embark_target = possible_embark;
			((Building*)possible_embark)->explore();
		}
	}


}

void FlightScene::render()
{
	float expl_power = 0.0f;
	for (int i = 0; i < expl_effects.size(); i++)
	{
		if (expl_effects[i].t >= 0.0f)
		{
			expl_power += expl_effects[i].t / expl_effects[i].dist;
		}
	}

	if (expl_power > 1.0f)
	{
		expl_power = 1.0f;
	}

	int expl_poweri = (int)(expl_power * 100.0f);

	TCODConsole::root->setDefaultBackground(TCODColor(4 + expl_poweri, 24 + expl_poweri, 30 + expl_poweri));
	TCODConsole::root->clear();
	TCODConsole::root->setDefaultBackground(TCODColor::black);

	int vx = (WIDTH - vehicle.width) / 2;
	int vy = 40 / 2 - vehicle.height / 2;
	vehicle.draw(TCODConsole::root, vx, vy);

	TCODConsole::root->rect(0, 40, WIDTH, HEIGHT, true);

	status->draw(1, HEIGHT * 0.8, WIDTH - 1, HEIGHT - 1);

	// Draw frames
	for (int i = 0; i < WIDTH; i++)
	{
		TCODConsole::root->setChar(i, 40, TCOD_CHAR_DHLINE);
	}

	// Draw window
	vehicle.draw_window(TCODConsole::root);


	if (possible_embark != nullptr && vehicle.blink)
	{
		TCODConsole::root->setDefaultForeground(TCODColor::lightYellow);
		TCODConsole::root->printf(0, 0, "Embark is possible");
		TCODConsole::root->printf(0, 1, "Press 'g' to embark!");
		TCODConsole::root->setDefaultForeground(TCODColor::white);
	}

}

void FlightScene::explosion(float x, float y, float power)
{
	for (int i = 0; i < map.entities.size(); i++)
	{
		float dx = map.entities[i]->get_x() - x;
		float dy = map.entities[i]->get_y() - y;

		float dist = sqrt(dx * dx + dy * dy);
		
		if (dist < 0.1f)
		{
			dist = 0.1f;
		}

		float damage = power / dist;

		if (damage > 1.0f && map.entities[i]->is_alive())
		{
			map.entities[i]->damage(damage - 0.25f);
		}
	}

	// Play sound 
	float dx = vehicle.x - x;
	float dy = vehicle.y - y;
	float dist = sqrt(dx * dx + dy * dy);
	if (dist < 0.15f)
	{
		dist = 0.15f;
	}

	if (dist >= 1.5f)
	{
		g_soloud->play(explo_far_dist, 2.0f);
	}
	else if (dist >= 0.6f)
	{
		g_soloud->play(explo_med_dist, 2.0f);
	}
	else
	{
		g_soloud->play(explo_near_dist, 1.0f);

		if (dist <= 0.4f)
		{
			g_soloud->play(explo_hit, 2.5f);
			// Damage the vehicle
			vehicle.damage(power * 25.0f / (dist / 0.15f));
		}
	}

	ExplosionEffect fx;
	fx.x = x;
	fx.y = y;
	fx.t = power * 2.0f;
	fx.dist = dist;
	expl_effects.push_back(fx);
}

void FlightScene::shut_up()
{
	underwater.setVolume(0.0f);
}



FlightScene::FlightScene() : map(map_size, map_size, 11234, this), vehicle(&map)
{
	embark_target = nullptr;

	underwater.load("underwater.wav");
	underwater.setLooping(true);

	explo_far_dist.load("explo_far_dist.wav");
	explo_med_dist.load("explo_med_dist.wav");
	explo_near_dist.load("explo_near_dist.wav");
	explo_hit.load("collide.wav");

	this->soloud = g_soloud;
	this->status = g_status;

	soloud->play(underwater);

	vehicle.scene = this;
}


FlightScene::~FlightScene()
{
}
