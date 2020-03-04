#pragma once
#include "soloud.h"
#include "Status.h"

#define WIDTH 90
#define HEIGHT 70

constexpr float PI = 3.1415926535f;

extern SoLoud::Soloud* g_soloud;
extern Status* g_status;

extern TCOD_key_t g_key;

extern TCODRandom* g_random;

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