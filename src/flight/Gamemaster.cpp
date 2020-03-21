#include "Gamemaster.h"
#include "FlightScene.h"
#include "../disembark/EmbarkScene.h"
#include "entity/Buildings.h"


void Gamemaster::update(float dt)
{
	if (!gameover)
	{
		if (!embarked)
		{
			flight_scene->update(dt);
		}
		else
		{
			embark_scene->update(dt);
		}
	}

	if (embarked)
	{
		flight_scene->shut_up();

		if (embark_scene->finished)
		{
			flight_scene->embark_target = nullptr;

			if (g_master->clear_nest_count == 2)
			{
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

				flight_scene->vehicle.radio->push_message("\
Greetings from the fatherland. We have received information from other submarines about strange and massive\
 structures. You may have already seen some of them. They look like a tower on the sonar.\n\n\
One of these has been reported on coordinates: " + coords + "\n\n\
You should continue clearing nests, but it's of our interest to get more information about these structures.\n\
We don't know what dangers may lie there, be careful, commander.");

			}
			else if (g_master->clear_nest_count == 3)
			{
				flight_scene->vehicle.radio->push_message("\
Greetings from the fatherland. Our leader has given orders to all submarines to explore these huge structures.\n\
Keep your vehicle focused on this task.\n\
Proceed with caution, commander.");
			}

			embarked = false;
			int alive = 0;
			bool reassign_captain = false;
			std::vector<std::string> dead;
			// Handle crew changes (crew could have died)
			for (EmbarkCrew& c : embark_scene->crew)
			{
				if (c.health <= 0.0f)
				{
					// Remove from vehicle
					auto& vcrew = flight_scene->vehicle.crew;
					dead.push_back(c.gc->name);
					for (auto it = vcrew.begin(); it != vcrew.end(); )
					{
						if (it->gc == c.gc)
						{
							if (it->gc->is_captain)
							{
								reassign_captain = true;
							}
							it = vcrew.erase(it);
						}
						else
						{
							it++;
						}
					}

					// Remove from crew
					for (auto it = crew.begin(); it != crew.end(); )
					{
						if (*it == c.gc)
						{
							it = crew.erase(it);
						}
						else
						{
							it++;
						}
					}

				}
				else
				{
					alive++;
				}
			}

			if (alive > 0)
			{

				std::string memory;

				if (dead.size() == 0)
				{
					memory = "Thanksfully, all the expedition crew survived.";
				}
				else
				{
					std::string name = "expeditioners were";
					if (dead.size() == 1)
					{
						name = "expeditioner was";
					}

					memory = "Sadly, " + std::to_string(dead.size()) + " " + name + " lost. May their names be remembered: \n\n";
					for (int i = 0; i < dead.size(); i++)
					{
						memory += dead[i];
						memory += '\n';
					}
				}

				g_popup->show("The crew made its way back into the\
 submarine through the hole they had originally blown up.\n\n" + memory);


			}
			else
			{
				g_popup->show("No response was received from the expedition crew, vitals were lost. May they rest in peace.");
			}

			if (flight_scene->vehicle.crew.size() == 0)
			{
				gameover = true;
			}
			else
			{
				if (reassign_captain)
				{
					flight_scene->vehicle.crew[0].gc->is_captain = true;
				}
			}


		}
	}
	else
	{
		if (flight_scene->embark_target != nullptr)
		{
			if (flight_scene->embark_target->get_type() == E_NEST)
			{
				g_popup->show("Four of the crewmembers left the vehicle in a tiny auxiliary submarine, carrying\
 a few hundred kilograms of explosives to breach a hole. They set the bomb and detonated it, opening a reasonably\
 sized hole in the fleshy wall of the nest. The crew quickly inserted a clamp as the hole was closing rapidly.\n\
As the flesh around the hole healed, they prepared to jump inside.\n\n\
The explosion has been intense, the aliens inside are probably not too happy!");

				g_master->clear_nest_count++;
			}
			else if (flight_scene->embark_target->get_type() == E_STATION)
			{
				if (g_master->clear_station_count < 2)
				{
					g_popup->show("Four of the crewmembers left the vehicle in a tiny auxiliary submarine, and approached\
 what seemed to be the entrance of the huge structure. It spanned a few kilometers vertically, reaching the\
 topmost ice crust, and touching the rock layer below the ocean.\n\
Upon reaching the entrance, they realized that it was clogged up, so they set a small\
 breaching charge and blew a small hole. The interior of the structure, unlike its exterior, was perfectly clean.\
 The walls were covered in instruments and lights, which illuminated the room with a faint blue colour.\n\n\
The explosion was probably heard around, so prepare for action!");

				}
				else
				{
					g_popup->show("Four of the crewmembers left the vehicle in a tiny auxiliary submarine, and approached\
 what seemed to be the entrance of the huge structure. It spanned a few kilometers vertically, reaching the\
 topmost ice crust, and touching the rock layer below the ocean.\n\n\
Unlike the previous stations they had explored, this one was way cleaner, runes engraved in the rock were clearly visible.\n\
Upon reaching the entrance, they realized that it was clogged up, so they set a small\
 breaching charge and blew a small hole. The interior of the structure, unlike its exterior, was perfectly clean.\
 The walls were covered in instruments and lights, which illuminated the room with a faint blue colour.\n\n\
The explosion was probably heard around, so prepare for action!");
				}
				

				g_master->clear_station_count++;
				std::cout << "c = " << g_master->clear_station_count << std::endl;
			}

			embark_scene->restart();
			Building* as_building = (Building*)flight_scene->embark_target;

			embark_scene->map = &as_building->get_bmap();

			// Obtain 4 expedition crew
			int cnum = 1;
			EmbarkCrew captain = EmbarkCrew();
			captain.gc = flight_scene->vehicle.get_captain()->gc;
			embark_scene->crew.push_back(captain);
			for (int i = 0; i < crew.size(); i++)
			{
				bool found = false;

				for (int j = 0; j < embark_scene->crew.size(); j++)
				{
					if (crew[i] == embark_scene->crew[j].gc)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{

					cnum++;
					if (cnum > 4)
					{
						break;
					}

					EmbarkCrew ec = EmbarkCrew();
					ec.gc = crew[i];
					embark_scene->crew.push_back(ec);
				}
			}

			embarked = true;
			flight_scene->embark_target = nullptr;

			embark_scene->update(0.0f);
		}
	}
}

void Gamemaster::init()
{
	// Load crew from the vehicle
	for (int i = 0; i < flight_scene->vehicle.crew.size(); i++)
	{
		crew.push_back(flight_scene->vehicle.crew[i].gc);
	}

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

}

Gamemaster::Gamemaster()
{
	seen_robot = false;
	seen_grunt = false;
	clear_station_count = 0;
	clear_nest_count = 0;
}

Gamemaster::~Gamemaster()
{
}
