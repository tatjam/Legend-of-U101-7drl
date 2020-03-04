#pragma once
#include "libtcod.h"
#include "soloud_wav.h"
#include "defines.h"

static SoLoud::Wav* click = nullptr;

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
	static bool draw_button(TCODConsole* target, int x0, int y0, int rx, int ry, int ch)
	{
		TCOD_mouse_t pos = TCODMouse::getStatus();

		int w = 2;
		int h = 2;

		bool clicked = false;
		if (pos.cx - rx >= x0 && pos.cx - rx <= x0 + w && pos.cy - ry >= y0 && pos.cy - ry <= y0 + h && pos.lbutton)
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
};