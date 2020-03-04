#include "FlightScene.h"




void FlightScene::update(float dt)
{
	t += dt;

	if (g_key.vk == TCODK_F11)
	{
		help_open = !help_open;
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

	if (g_key.vk == TCODK_KP8)
	{
		vehicle.move_order(N);
	}

	if (g_key.vk == TCODK_KP2)
	{
		vehicle.move_order(S);
	}

	if (g_key.vk == TCODK_KP6)
	{
		vehicle.move_order(E);
	}

	if (g_key.vk == TCODK_KP4)
	{
		vehicle.move_order(W);
	}

	if (g_key.vk == TCODK_KP1)
	{
		vehicle.move_order(SW);
	}

	if (g_key.vk == TCODK_KP7)
	{
		vehicle.move_order(NW);
	}

	if (g_key.vk == TCODK_KP9)
	{
		vehicle.move_order(NE);
	}

	if (g_key.vk == TCODK_KP3)
	{
		vehicle.move_order(SE);
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





}

void FlightScene::render()
{
	TCODConsole::root->setDefaultBackground(TCODColor(4, 24, 30));
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

	
	if (help_open)
	{
		Drawing::draw_window(TCODConsole::root, 0, 0, 40, HEIGHT - 1, "Help", true);
		
		TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
		TCODConsole::root->printRect(1, 1, 39, HEIGHT - 2, help_str.c_str());
	}



}



FlightScene::FlightScene() : map(map_size, map_size, 11234), vehicle(&map)
{
	underwater.load("underwater.wav");
	underwater.setLooping(true);

	this->soloud = g_soloud;
	this->status = g_status;

	soloud->play(underwater);

	help_str = Help::help_main;
}


FlightScene::~FlightScene()
{
}
