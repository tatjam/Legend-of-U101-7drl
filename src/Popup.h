#pragma once
#include "Drawing.h"


class Popup
{
public:

	bool is_shown;
	std::string txt;
	void show(const std::string& str)
	{
		is_shown = true;
		txt = str;
	}

	void update(float dt)
	{
		if (g_key.vk == TCODK_ESCAPE || g_key.vk == TCODK_ENTER)
		{
			is_shown = false;
		}
	}

	void draw() 
	{
		Drawing::draw_window(TCODConsole::root, 20, 10, 50, 30, "Attention!", true);
		TCODConsole::root->printRect(22, 12, 47, 27, txt.c_str());
	}


};

