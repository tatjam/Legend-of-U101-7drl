#include "Radio.h"



void Radio::draw(int rx, int ry)
{
	console.clear();

	Drawing::draw_rectangle(&console, 1, 1, console.getWidth() - 4, console.getHeight() - 8);

	if (messages.size() != 0)
	{
		console.printRect(2, 2, console.getWidth() - 5, console.getHeight() - 9, messages[msg].c_str());

		if (Drawing::draw_button(&console, console.getWidth() - 9, console.getHeight() - 10, rx, ry, 186))
		{
			if (g_soloud->isValidVoiceHandle(handle))
			{
				g_soloud->stop(handle);
			}
		}

		if (Drawing::draw_button(&console, console.getWidth() - 6, console.getHeight() - 10, rx, ry, 62))
		{
			say_message(messages[msg]);
		}
	}

	if (Drawing::draw_button(&console, 1, console.getHeight() - 5, rx, ry, 174, true))
	{
		msg = 0;
	}

	if (Drawing::draw_button(&console, 4, console.getHeight() - 5, rx, ry, '<', true))
	{
		msg--;
		if (msg < 0)
		{
			msg = 0;
		}
	}

	if (Drawing::draw_button(&console, console.getWidth() - 9, console.getHeight() - 5, rx, ry, '>', true))
	{
		msg++;
		if (msg > messages.size() - 1)
		{
			msg = messages.size() - 1;
		}
	}

	if (Drawing::draw_button(&console, console.getWidth() - 6, console.getHeight() - 5, rx, ry, 175, true))
	{
		msg = messages.size() - 1;
		if (msg < 0)
		{
			msg = 0;
		}
	}

	

	console.setAlignment(TCOD_CENTER);
	console.printf(console.getWidth() / 2, console.getHeight() - 4, "%i/%i", msg + 1, messages.size());
	console.setAlignment(TCOD_LEFT);
}

Radio::Radio() : console(70, 50)
{
	msg = 0;

	voice_in.setParams(1500, 9.0f, 0.5f, KW_NOISE);

	
	distort.load("distort.wav");
	distort.setLooping(true);
	distort_handle = g_soloud->play(distort, 0.0f);
}

void Radio::say_message(std::string& msg)
{

	voice_in.setText((msg + ". . . Out.").c_str());

	handle = g_soloud->play(voice_in, 7.0f);

}

void Radio::push_message(std::string nmsg)
{


	messages.push_back(nmsg);
	dirty = true;

	// If anybody is on the radio, say new message received
	if (is_crewed() && !is_open())
	{
		get_crewman()->gc->speak("Captain, new radio message");
	}

	msg = messages.size() - 1;
	
}

Radio::~Radio()
{
}



bool Radio::update(float dt)
{
	if (g_key.vk == TCODK_ESCAPE) 
	{
		return true;
	}

	if (g_soloud->isValidVoiceHandle(handle))
	{
		g_soloud->setVolume(distort_handle, 0.6f);
	}
	else
	{
		g_soloud->setVolume(distort_handle, 0.0f);
	}

	if (is_open() && dirty)
	{
		say_message(*(messages.end() - 1));
		dirty = false;
	}

	return false;
}
