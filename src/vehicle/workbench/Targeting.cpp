#include "Targeting.h"
#include "../../flight/entity/EntityTorpedo.h"
#include "../Vehicle.h"
#include "../../flight/FlightScene.h"

void Targeting::fire_torpedo()
{
	if (is_crewed())
	{
		if (target == nullptr)
		{
			get_crewman()->gc->speak("No target assigned!");
		}
		else
		{
			if (has_torpedo())
			{
				if (torpedo_timer < 0.0f)
				{
					get_crewman()->gc->speak("Firing torpedo!");
					torpedo_timer = 4.0f;
					g_soloud->play(torpedo_out, 6.0f);
					firing = true;
				}
			}
			else
			{
				get_crewman()->gc->speak("No torpedoes loaded!");
			}
		}
	}
}

bool Targeting::has_torpedo()
{
	for (int i = 0; i < get_vehicle()->torpedo_slots.size(); i++)
	{
		if (get_vehicle()->torpedo_slots[i].has_torpedo)
		{
			return true;
		}
	}

	return false;
}

Targeting::Targeting() : console(50, 50)
{
	torpedo_timer = 0.0f;
	torpedo_out.load("torpedo_out.wav");
	target = nullptr;

	identify = nullptr;
	identify_timer = 0.0f;
	firing = false;
	said = false;
}


Targeting::~Targeting()
{
}

std::vector<FlightEntity*> Targeting::get_visible_entities()
{
	std::vector<FlightEntity*> out;

	for (int i = 0; i < get_vehicle()->in_map->entities.size(); i++)
	{
		FlightEntity* ent = get_vehicle()->in_map->entities[i];
		if (ent->is_visible() && 
			ent->get_type() != E_BASE && ent->get_type() != E_NEST &&
			ent->get_type() != E_STATION && ent->get_type() != E_TORPEDO
			&& ent->is_alive())
		{
			out.push_back(get_vehicle()->in_map->entities[i]);
		}
	}

	return out;
}

bool Targeting::update(float dt)
{

	torpedo_timer -= dt;
	
	if (torpedo_timer < 1.8f)
	{
		if (firing)
		{
			// Fire the boy
			EntityTorpedo* torpedo = new EntityTorpedo(target);
			torpedo->velocity = 0.35f;
			torpedo->heading = get_vehicle()->angle;
			torpedo->safety = 1.0f;
			torpedo->maneouver = 0.74f;
			torpedo->lifetime = 25.0f;

			torpedo->set_position(get_vehicle()->x, get_vehicle()->y);
			torpedo->init(get_vehicle()->in_map, get_vehicle()->scene);


			get_vehicle()->in_map->entities.push_back(torpedo);

			for (int i = 0; i < get_vehicle()->torpedo_slots.size(); i++)
			{
				if (get_vehicle()->torpedo_slots[i].has_torpedo)
				{
					get_vehicle()->torpedo_slots[i].has_torpedo = false;
					break;
				}
			}

			firing = false;
		}
	}

	if (torpedo_timer < 0.0f)
	{
		if (!said)
		{
			if (is_crewed() && has_torpedo())
			{
				get_crewman()->gc->speak("Weapons ready!");
				said = true;
			}
		}
	}
	else
	{
		said = false;
	}

	if (is_crewed())
	{
		identify_timer -= dt;

		auto visibles = get_visible_entities();

		if (identify != nullptr)
		{
			bool found = false;
			for (int i = 0; i < visibles.size(); i++)
			{
				if (visibles[i] == identify)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				get_crewman()->gc->speak("Sonar target lost");
				identify = nullptr;
				identify_timer = 0.0f;
			}
		}

		if (identify_timer <= 0.0f)
		{
			if (identify != nullptr)
			{
				identify->identify();

				std::string name = identify->get_name();

				get_crewman()->gc->speak("Identified a " + name);
				identify = nullptr;
			}

			// Automatically start finding a new one
			for (int i = 0; i < visibles.size(); i++)
			{
				if (!visibles[i]->is_identified())
				{
					identify = visibles[i];
					identify_timer = IDENTIFY_TIME;
				}
			}
		}
	}

	return false;
}

void Targeting::draw(int rx, int ry)
{
	console.clear();

	// Draw a list of visible entities, and show a identification 
	// progress meter. The user can click entities to target
	// and it shows a little preview of the entity in a sonar

	auto visible = get_visible_entities();

	std::vector<FlightEntity*> sorted;

	// We first sort
	for (int i = 0; i < visible.size(); i++)
	{
		if (visible[i]->is_identified())
		{
			sorted.push_back(visible[i]);
		}
	}

	for (int i = 0; i < visible.size(); i++)
	{
		if (!visible[i]->is_identified())
		{
			sorted.push_back(visible[i]);
		}
	}

	int selected = -1;
	int highlighted = -1;

	TCOD_mouse_t mouse = TCODMouse::getStatus();

	// Now draw the list
	for (int i = 0; i < sorted.size(); i++)
	{
		if (mouse.cy - ry == i + 2)
		{
			highlighted = i;
		}

		if (highlighted == i)
		{
			console.putChar(2, i + 2, '>');
			if (mouse.lbutton_pressed)
			{
				if (target != sorted[i])
				{
					target = sorted[i];
					get_crewman()->gc->speak("Changed weapon's target");
				}
				
			}

			if (mouse.rbutton_pressed)
			{
				if (identify != sorted[i] && !sorted[i]->is_identified())
				{
					identify = sorted[i];
					identify_timer = IDENTIFY_TIME;
					get_crewman()->gc->speak("Identifying sonar signal");
				}
			}
		}

		if (identify == sorted[i])
		{
			console.setChar(0, i + 2, 'I');
			console.setCharForeground(0, i + 2, TCODColor(255, 191, 0));
		}

		if (target == sorted[i])
		{
			console.setChar(1, i + 2, 'T');
			console.setCharForeground(1, i + 2, TCODColor(255, 50, 0));
		}

		if (sorted[i]->is_identified())
		{
			if (sorted[i]->get_type() != E_SUBMARINE)
			{
				console.setDefaultForeground(TCODColor(255, 128, 128));
			}
			else
			{
				console.setDefaultForeground(TCODColor(128, 255, 128));
			}
		}


		if (sorted[i]->is_identified())
		{
			console.putChar(3, i + 2, sorted[i]->get_symbol());
		}
		else
		{ 
			console.putChar(3, i + 2, '?');
		}

		int xo = 3;

		console.setDefaultForeground(TCODColor::white);



		if (sorted[i]->is_identified())
		{
			console.printf(2 + xo, i + 2, sorted[i]->get_name().c_str());
		}
		else
		{
			console.printf(2 + xo, i + 2, "Unknown Signal");
		}

		xo = 19;

		// Distance 
		float dx = sorted[i]->get_x() - get_vehicle()->x;
		float dy = sorted[i]->get_y() - get_vehicle()->y;

		float dist = sqrt(dx * dx + dy * dy);

		std::string str = "";
		str += std::to_string((int)round(dist * 100.0f));
		str += "m";

		console.printf(2 + xo, i + 2, str.c_str());



	}

	console.setDefaultForeground(TCODColor::grey);
	// Draw small sonar
	Drawing::draw_rectangle(&console, 32, -1, 32, 17, true);

	console.setDefaultForeground(TCODColor::white);


	int sx, sy;
	if (highlighted == -1)
	{
		sx = 16;
		sy = 16;
	}
	else
	{
		float ex = sorted[highlighted]->get_x();
		float ey = sorted[highlighted]->get_y();

		// Relative to the player
		float rx = ex - get_vehicle()->x;
		float ry = ey - get_vehicle()->y;

		// Relative to the sonar upper left corner
		float ssx = rx + 24.0f;
		float ssy = ry + 24.0f;

		sx = (int)ssx - 7;
		sy = (int)ssy - 7;

	}

	if (get_vehicle()->sonar->is_crewed())
	{
		get_vehicle()->sonar->draw(0, 0);
		TCODConsole::blit(&get_vehicle()->sonar->console, sx, sy, 16, 16, &console, 33, 0);
	}
	


}

TCODConsole* Targeting::get_console()
{
	return &console;
}
