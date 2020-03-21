#pragma once
#include "BuildingMap.h"
#include "../Crewmember.h"

class EmbarkScene;

struct EmbarkCrew
{
	Crewmember* gc;
	EmbarkMonster* target;

	int x, y;

	float health;

	bool hold_fire;
	int ammo;
	int grenades;

	float reload_timer;
	float fire_timer;

	TCODPath* to_walk;
	float walk_t;

	float fired_timer;
	int fx, fy;

	std::vector<bool> view_map;

	void hurt(float dmg, EmbarkScene* scene);

	EmbarkCrew()
	{
		health = 1.0f;
		hold_fire = false;
		ammo = 8;
		grenades = g_random->getInt(1, 3);
		reload_timer = 0.0f;
		gc = nullptr;
		walk_t = 0.0f;
		to_walk = nullptr;
		target = nullptr;
		fire_timer = 0.0f;
		fired_timer = 0.0f;
	}

	bool fire(int tx, int ty, float dt, SoLoud::Wav* sound);

};

struct Grenade
{
	int x, y;
	float timer;
	bool exploded;

};

class EmbarkScene
{
public:

	bool finished;

	float t;

	bool mission_done;

	void splatter(int x, int y, bool human);

	void update_entity(float dt, EmbarkMonster* monster);

	void spot(int x, int y, EmbarkCrew* crew);

	bool blink;
	float blinkt;


	EmbarkCrew* selected;

	void handle_mouse();
	void update_crew(float dt, EmbarkCrew* crew);

	SoLoud::Wav gunshot;
	SoLoud::Wav grenade;
	SoLoud::Wav grenade_land;
	SoLoud::Wav hurt;
	SoLoud::Wav hurt_alien;
	SoLoud::Wav hurt_metal;
	SoLoud::Wav die_alien;
	SoLoud::Wav die_man;
	SoLoud::Wav die_metal;


	bool is_init;

	std::vector<EmbarkCrew> crew;
	std::vector<Grenade> grenades;

	BuildingMap* map;

	void update(float dt);
	void render();

	bool is_free(int x, int y, int ignore_ent = -1);

	bool is_free_crew(int x, int y);

	void restart();

	EmbarkScene();
	~EmbarkScene();
};

