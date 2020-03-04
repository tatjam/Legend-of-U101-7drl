#include "libtcod.hpp"
#include "defines.h"

#include "Status.h"
#include "flight/FlightScene.h"

#include "soloud/include/soloud.h"
#include "soloud/include/soloud_wav.h"

SoLoud::Soloud* g_soloud;
Status* g_status;
TCOD_key_t g_key;
TCODRandom* g_random;

int main(int argc, char** argv)
{

	TCODConsole::setCustomFont("rc11.png", TCOD_FONT_LAYOUT_ASCII_INROW);
	TCODConsole::initRoot(WIDTH, HEIGHT, "7DRL", false, TCOD_RENDERER_GLSL);


	g_soloud = new SoLoud::Soloud;
	g_soloud->init();

	g_status = new Status();
	g_random = new TCODRandom();

	FlightScene flight_scene = FlightScene();

	TCODSystem::setFps(60);

	while (!TCODConsole::isWindowClosed()) 
	{
		TCOD_mouse_t mouse;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &g_key, &mouse);

		flight_scene.update(TCODSystem::getLastFrameLength());

	

		TCODConsole::root->clear();
		flight_scene.render();
		TCODConsole::flush();
	}

	g_soloud->deinit();

	return 0;
}
