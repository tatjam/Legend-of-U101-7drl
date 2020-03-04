#include "Crewmember.h"
#include "workbench/Workbench.h"

static SoLoud::Wav* radio = nullptr;


void Crewmember::speak(std::string text)
{
	if (radio == nullptr)
	{
		radio = new SoLoud::Wav();
		radio->load("radio.wav");
	}

	g_soloud->play(*radio);

	voice.setText((". . ." + text).c_str());
	g_soloud->play(voice, 6.0f);

	g_status->strings.push_back(name + ": " + text);
}

Crewmember::Crewmember(std::string name, int voice_pitch, float voice_speed, int voice_oscillator)
{
	path = nullptr;
	path_t = 0.0f;
	this->name = name;
	
	voice = SoLoud::Speech();
	voice.setParams(voice_pitch, voice_speed, 0.5f, voice_oscillator);
}

Crewmember::Crewmember()
{
	path = nullptr;
	path_t = 0.0f;
	TCODRandom rng = TCODRandom();
	// Procedural

	std::array<std::string, 15> first_names =
	{
		"James",
		"John",
		"Robert",
		"Michael",
		"William",
		"David",
		"Richard",
		"Joseph",
		"Guamedo",
		"Thomas",
		"Charles",
		"Donald",
		"Mark",
		"Brian",
		"Alex"
	};

	std::array<std::string, 17> last_names =
	{ 
		"Smith",
		"Johnson",
		"Williams",
		"Jones",
		"Brown",
		"Davis",
		"Miller",
		"Wilson",
		"Moore",
		"Taylor",
		"Anderson",
		"Thomas",
		"Jackson",
		"White",
		"Garcia",
		"Martinez",
		"Young"
	};

	name = first_names[rand() % first_names.size()] + " " + last_names[rand() % last_names.size()];
	
	int wave = KW_TRIANGLE;
	if (rng.getFloat(0.0f, 1.0f) >= 0.5f) 
	{
		wave = KW_PULSE;
	}
	else if(rng.getFloat(0.0f, 1.0f) >= 0.5f)
	{
		wave = KW_WARBLE;
	}
	else if(rng.getFloat(0.0f, 1.0f) >= 0.5f)
	{
		wave = KW_SQUARE;
	}

	voice.setParams(rng.getInt(900, 1900), rng.getFloat(7.0f, 10.0f), rng.getFloat(0.4f, 0.6f), wave);
}

bool Crewmember::can_work_in(Workbench * bench)
{
	/*for (size_t i = 0; i < assigned.size(); i++)
	{
		if (bench == assigned[i])
		{
			return true;
		}
	}
	
	return false;
	*/

	return !is_captain;
}

void Crewmember::path_to(int dx, int dy, TCODMap& map)
{
	if (path != nullptr)
	{
		delete path;
		path = nullptr;
	}
	path = new TCODPath(&map);
	if (!path->compute(x, y, dx, dy))
	{
		delete path;
		path = nullptr;
	}
}
