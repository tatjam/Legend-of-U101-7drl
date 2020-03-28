#pragma once

#include "Status.h"



namespace SoLoud
{
	class Soloud;
}

class Gamemaster;
class Popup;

#define WIDTH 90
#define HEIGHT 70

constexpr float PI = 3.1415926535f;

extern SoLoud::Soloud* g_soloud;
extern Status* g_status;

extern TCOD_key_t g_key;

extern TCODRandom* g_random;

extern Gamemaster* g_master;
extern Popup* g_popup;

enum Direction
{
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW
};

enum Speed
{
	STOP,
	SLOW,
	MEDIUM,
	FAST,
	FULL
};

static float min(float a, float b)
{
	return a > b ? b : a;
}

static float max(float a, float b)
{
	return a > b ? a : b;
}
