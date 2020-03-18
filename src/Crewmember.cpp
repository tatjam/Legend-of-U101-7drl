#include "Crewmember.h"

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
	g_soloud->play(voice, 1.6f);

	g_status->strings.push_back(name + ": " + text);
}


Crewmember::Crewmember()
{
	std::array<std::string, 15> first_names =
	{
		"Bob",
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
		"Bobbins",
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
	if (g_random->getFloat(0.0f, 1.0f) >= 0.5f)
	{
		wave = KW_PULSE;
	}
	else if (g_random->getFloat(0.0f, 1.0f) >= 0.5f)
	{
		wave = KW_WARBLE;
	}
	else if (g_random->getFloat(0.0f, 1.0f) >= 0.5f)
	{
		wave = KW_SQUARE;
	}

	voice.setParams(g_random->getInt(900, 1900), g_random->getFloat(6.5f, 8.5f), g_random->getFloat(0.4f, 0.6f), wave);

}