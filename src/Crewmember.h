#pragma once
#include <string>
#include "defines.h"
#include <soloud_speech.h>
#include <soloud_wav.h>

class Crewmember
{
public:
	SoLoud::Speech voice;
	std::string name;
	bool is_captain;


	void speak(std::string text);

	Crewmember();
};

