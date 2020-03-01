#include "FlightScene.h"



void FlightScene::turn(TCOD_key_t key)
{

	if (open_window == SONAR)
	{

		if (key.vk == TCODK_UP)
		{
			py--;
		}

		if (key.vk == TCODK_DOWN)
		{
			py++;
		}

		if (key.vk == TCODK_RIGHT)
		{
			px++;
		}

		if (key.vk == TCODK_LEFT)
		{
			px--;
		}

		sonar.draw_world(px, py, map);
	}

	if (key.vk == TCODK_F11)
	{
		help_open = !help_open;
	}
}

void FlightScene::update(float dt)
{
	sonar.update(dt);
}

void FlightScene::render()
{
	TCODConsole::blit(&spaceship, 0, 0, 
		spaceship.getWidth(), spaceship.getHeight(), TCODConsole::root, sonar.console.getWidth(), 0);



	status->draw(1, sonar.console.getHeight(), WIDTH - 1, HEIGHT - 1);

	// Draw frames
	for (int i = 0; i < WIDTH; i++)
	{
		TCODConsole::root->setChar(i, 49, TCOD_CHAR_DHLINE);
	}

	if (open_window == SONAR)
	{

		int x0 = (TCODConsole::root->getWidth() - sonar.console.getWidth()) / 2;
		int y0 = (TCODConsole::root->getHeight() - sonar.console.getHeight()) / 2;

		sonar.draw(x0, y0);


		Drawing::draw_window(TCODConsole::root, x0 - 1, y0 - 1, sonar.console.getWidth(), sonar.console.getHeight(), "Sonar");

		TCODConsole::blit(&sonar.console, 0, 0,
			sonar.console.getWidth() - 1, sonar.console.getHeight() - 1, TCODConsole::root, x0, y0);
	}

	if (help_open)
	{
		Drawing::draw_window(TCODConsole::root, 0, 0, 40, HEIGHT - 1, "Help", true);
		
		TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
		TCODConsole::root->printRect(1, 1, 39, HEIGHT - 2, help_str.c_str());
	}



}


FlightScene::FlightScene(Status* status) : 
	sonar(soloud), spaceship(WIDTH - 49, 49), map(map_size, map_size, 11234)
{
	underwater.load("underwater.wav");
	underwater.setLooping(true);

	this->soloud = g_soloud;
	this->status = status;

	px = map_size / 2;
	py = map_size / 2;

	sonar.sonar_active = true;
	sonar.draw_world(px, py, map);



	soloud->play(underwater);

	open_window = SONAR;

	help_str = Help::help_main;
}


FlightScene::~FlightScene()
{
}
