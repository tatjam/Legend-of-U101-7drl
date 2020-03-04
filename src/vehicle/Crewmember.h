#pragma once
#include "libtcod.h"
#include "soloud.h"
#include "soloud_speech.h"
#include "soloud_wav.h"
#include "../defines.h"


#include <string>
#include <array>

class Workbench;




class Crewmember
{
public:

	SoLoud::Speech voice;

	std::string name;

	int x, y;

	bool is_captain;

	// The first one is priority (index 0)
	std::vector<Workbench*> assigned;

	void speak(std::string text);

	Crewmember(std::string name, int voice_pitch, float voice_speed, int voice_oscillator);
	Crewmember();

	bool can_work_in(Workbench* bench);

	void path_to(int dx, int dy, TCODMap& map);

	TCODPath* path;
	float path_t = 0.0f;
};