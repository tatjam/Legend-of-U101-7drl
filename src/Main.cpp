#include "libtcod.hpp"
#include "defines.h"

#include "Status.h"
#include "flight/FlightScene.h"
#include "disembark/EmbarkScene.h"

#include "soloud/include/soloud.h"
#include "soloud/include/soloud_wav.h"

#include "flight/entity/Buildings.h"
#include "flight/Gamemaster.h"
#include "Popup.h"

SoLoud::Soloud* g_soloud;
Status* g_status;
TCOD_key_t g_key;
TCODRandom* g_random;
Gamemaster* g_master;
Popup* g_popup;

int main(int argc, char** argv)
{

	TCODConsole::setCustomFont("rc11.png", TCOD_FONT_LAYOUT_ASCII_INROW);
	TCODConsole::initRoot(WIDTH, HEIGHT, "The Legend of U-101", false, TCOD_RENDERER_SDL);

	g_soloud = new SoLoud::Soloud;
	g_soloud->init();

	g_status = new Status();
	g_random = new TCODRandom();

	g_master = new Gamemaster();
	g_popup = new Popup();



	FlightScene flight_scene = FlightScene();
	EmbarkScene embark_scene = EmbarkScene();

	flight_scene.update(1.0f);

	g_master->flight_scene = &flight_scene;
	g_master->embark_scene = &embark_scene;
	g_master->embarked = false;

	g_master->init();

	TCODSystem::setFps(60);


	Help help = Help();

	bool help_open = false;
	bool paused = false;

	bool show_fps = false;

	g_popup->show("Press the F1 key to show the in-game help menu!\n\n\
You can press ESCAPE or ENTER to close these pop-ups.");

	while (!TCODConsole::isWindowClosed()) 
	{
		TCOD_mouse_t mouse;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &g_key, &mouse);

		if (g_key.vk == TCODK_F1)
		{
			help_open = !help_open;
		}

		if (g_key.vk == TCODK_SPACE)
		{
			paused = !paused;
		}

		float dt = TCODSystem::getLastFrameLength();
		float rdt = dt;
		if (paused || help_open || g_popup->is_shown)
		{
			rdt = 0.0f;
		}

		g_master->update(rdt);



		if (g_popup->is_shown)
		{
			g_popup->update(dt);
		}
		

		TCODConsole::root->clear();
		if (!g_master->embarked)
		{
			flight_scene.render();
		}
		else
		{
			embark_scene.render();
		}

		if (g_popup->is_shown)
		{
			g_popup->draw();
		}

		if (help_open)
		{
			help.draw();
		}

		if (g_key.vk == TCODK_F3)
		{
			show_fps = !show_fps;
		}

		if (show_fps)
		{
			TCODConsole::root->printf(32, 0, "FPS: %i", (int)(1.0f / dt));
		}

		if (g_master->gameover)
		{
			TCODConsole::root->setAlignment(TCOD_CENTER);
			TCODConsole::root->setDefaultForeground(TCODColor::lighterRed);
			TCODConsole::root->printf(WIDTH / 2, HEIGHT / 2 - 5, "Game Over");
			TCODConsole::root->setDefaultForeground(TCODColor::white);
			TCODConsole::root->printf(WIDTH / 2, HEIGHT / 2 - 4, "All crewmembers died");
			TCODConsole::root->setAlignment(TCOD_LEFT);
		}


		if (paused || help_open)
		{
			TCODConsole::root->setDefaultForeground(TCODColor::red);
			TCODConsole::root->setAlignment(TCOD_RIGHT);
			TCODConsole::root->printf(TCODConsole::root->getWidth() - 1, 0, "Paused");
			TCODConsole::root->setAlignment(TCOD_LEFT);
			TCODConsole::root->setDefaultForeground(TCODColor::white);
		}


		TCODConsole::flush();
	}

	g_soloud->deinit();

	return 0;
}
