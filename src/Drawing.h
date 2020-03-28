#pragma once
#include "libtcod.h"
#include "soloud_wav.h"
#include "defines.h"

static SoLoud::Wav* click = nullptr;
static SoLoud::Wav* four_way = nullptr;

class Drawing
{
public:


	static void draw_window(TCODConsole* target, int x0, int y0, int w, int h, const std::string& title,
		bool clear_inside = false)
	{
		TCODConsole::root->setDefaultForeground(TCODColor::grey);

		for (int x = x0; x < x0 + w; x++)
		{
			target->putChar(x, y0, TCOD_CHAR_DHLINE);
			target->putChar(x, y0 + h, TCOD_CHAR_DHLINE);
		}

		for (int y = y0; y < y0 + h; y++)
		{
			target->putChar(x0, y, TCOD_CHAR_DVLINE);
			target->putChar(x0 + w, y, TCOD_CHAR_DVLINE);
		}

		target->putChar(x0, y0, TCOD_CHAR_DNW);
		target->putChar(x0 + w, y0, TCOD_CHAR_DNE);
		target->putChar(x0, y0 + h, TCOD_CHAR_DSW);
		target->putChar(x0 + w, y0 + h, TCOD_CHAR_DSE);

		int tw = title.size();
		int tx = x0 + (w - tw) / 2;

		target->putChar(tx - 1, y0, 185);
		target->putChar(tx + tw, y0, 204);

		TCODConsole::root->setDefaultForeground(TCODColor::white);
		target->printf(tx, y0, title.c_str());

		if (clear_inside)
		{
			for (int x = x0 + 1; x < x0 + w; x++)
			{
				for (int y = y0 + 1; y < y0 + h; y++)
				{
					target->setChar(x, y, ' ');
					target->setCharForeground(x, y, TCODColor::white);
					target->setCharBackground(x, y, TCODColor::black);
				}
			}
		}
	}

	// Only changes foregrounds
	// Returns true if changed
	static bool draw_switch(TCODConsole* target, int x, int y, int rx, int ry, bool* on, const std::string& title)
	{
		if (click == nullptr)
		{
			click = new SoLoud::Wav();
			click->load("click.wav");
		}
		// LED
		target->setChar(x, y, '(');
		target->setChar(x+2, y, ')');
		target->setChar(x + 1, y, 254);

		// Indicators
		target->setChar(x, y + 2, '0');
		target->setChar(x + 2, y + 2, '1');

		// Lever
		target->setChar(x, y + 1, TCOD_CHAR_DHLINE);
		target->setChar(x + 1, y + 1, TCOD_CHAR_DHLINE);
		target->setChar(x + 2, y + 1, TCOD_CHAR_DHLINE);

		if (*on)
		{
			target->setChar(x + 2, y + 1, 'O');
		}
		else
		{
			target->setChar(x, y + 1, 'O');
		}

		if (*on)
		{
			target->setCharForeground(x + 1, y, TCODColor::lightRed);
		}
		else
		{
			target->setCharForeground(x + 1, y, TCODColor::grey);
		}

		int tw = title.size();
		int tx = (x + 1) - (tw / 2);
		target->printf(tx, y - 1, title.c_str());

		TCOD_mouse_t pos = TCODMouse::getStatus();

		if (pos.cx - rx >= x && pos.cx - rx <= x + 2 && pos.cy - ry == y + 1 && pos.lbutton_pressed)
		{
			*on = !(*on);
			g_soloud->play(*click);
			return true;
		}

		return false;
	}

	static void draw_rectangle(TCODConsole* target, int x0, int y0, int w, int h, bool dline = false)
	{
		int hline = 196;
		int vline = 179;
		int ulc = 218;
		int urc = 191;
		int blc = 192;
		int brc = 217;

		if (dline)
		{
			hline = 205;
			vline = 186;
			ulc = 201;
			urc = 187;
			blc = 200;
			brc = 188;
		}

		for (int x = x0; x < x0 + w; x++)
		{
			target->putChar(x, y0, hline);
			target->putChar(x, y0 + h, hline);
		}

		for (int y = y0; y < y0 + h; y++)
		{
			target->putChar(x0, y, vline);
			target->putChar(x0 + w, y, vline);
		}

		// Corners

		target->putChar(x0, y0, ulc);
		target->putChar(x0 + w, y0, urc);
		target->putChar(x0, y0 + h, blc);
		target->putChar(x0 + w, y0 + h, brc);
	}

	// Single letter hold button
	static bool draw_button(TCODConsole* target, int x0, int y0, int rx, int ry, int ch, bool one_click = false)
	{
		TCOD_mouse_t pos = TCODMouse::getStatus();

		int w = 2;
		int h = 2;


		bool clicked = false;
		bool button = false;
		if (one_click)
		{
			button = pos.lbutton_pressed;
		}
		else
		{
			button = pos.lbutton;
		}

		if (pos.cx - rx >= x0 && pos.cx - rx <= x0 + w && pos.cy - ry >= y0 && pos.cy - ry <= y0 + h && button)
		{
			clicked = true;
		}

		draw_rectangle(target, x0, y0, w, h, false);

		if (clicked)
		{
			target->setDefaultForeground(TCODColor::lightGreen);
		}
		else
		{
			target->setDefaultForeground(TCODColor::white);
		}

		target->setChar(x0 + w / 2, y0 + h / 2, ch);

	
		target->setDefaultForeground(TCODColor::white);

		return clicked;
	}

	// 0 = Top
	// 1 = Right
	// 2 = Bottom
	// 3 = Left
	// x,y are coordiantes of the four choice thing, not text
	static bool draw_four_choice(TCODConsole* target, int x, int y, int rx, int ry, int* dir,
		const std::string& opt_u,
		const std::string& opt_r,
		const std::string& opt_d,
		const std::string& opt_l)
	{
		if (four_way == nullptr)
		{
			four_way = new SoLoud::Wav();
			four_way->load("four_way.wav");
		}

		target->putChar(x + 1, y + 0, 201);
		target->putChar(x + 2, y + 0, 205);
		target->putChar(x + 3, y + 0, 187);

		target->putChar(x + 0, y + 1, 201);
		target->putChar(x + 1, y + 1, 188);
		target->putChar(x + 3, y + 1, 200);
		target->putChar(x + 4, y + 1, 187);

		target->putChar(x + 0, y + 2, 186);
		target->putChar(x + 2, y + 2, 7);
		target->putChar(x + 4, y + 2, 186);

		target->putChar(x + 0, y + 3, 200);
		target->putChar(x + 1, y + 3, 187);
		target->putChar(x + 3, y + 3, 201);
		target->putChar(x + 4, y + 3, 188);

		target->putChar(x + 1, y + 4, 200);
		target->putChar(x + 2, y + 4, 205);
		target->putChar(x + 3, y + 4, 188);

		if (*dir == 0)
		{
			target->putChar(x + 2, y + 1, 24);
		}
		else if (*dir == 1) 
		{
			target->putChar(x + 3, y + 2, 26);
		}
		else if (*dir == 2)
		{
			target->putChar(x + 2, y + 3, 25);
		}
		else
		{
			target->putChar(x + 1, y + 2, 27);
		}

		// Draw texts
		target->setAlignment(TCOD_CENTER);
		target->printf(x + 2, y - 1, opt_u.c_str());
		target->printf(x + 2, y + 5, opt_d.c_str());
		target->setAlignment(TCOD_RIGHT);
		target->printf(x - 1, y + 2, opt_l.c_str());
		target->setAlignment(TCOD_LEFT);
		target->printf(x + 5, y + 2, opt_r.c_str());

		// Interaction
		TCOD_mouse_t pos = TCODMouse::getStatus();

		int pos_cx = pos.cx - rx;
		int pos_cy = pos.cy - ry;

		int old_dir = *dir;

		if (pos.lbutton_pressed)
		{
			if (pos_cx == x + 2 && pos_cy == y + 1)
			{
				*dir = 0;
			}
			else if (pos_cx == x + 3 && pos_cy == y + 2)
			{
				*dir = 1;
			}
			else if (pos_cx == x + 2 && pos_cy == y + 3)
			{
				*dir = 2;
			}
			else if (pos_cx == x + 1 && pos_cy == y + 2)
			{
				*dir = 3;
			}
		}

		if (*dir != old_dir)
		{
			// Play sound
			g_soloud->play(*four_way);
			return true;
		}

		return false;
	}
};