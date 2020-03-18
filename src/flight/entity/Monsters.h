#pragma once

#include "FlightEntity.h"
#include <soloud_wav.h>

class Vehicle;

// Implements basic stuff for the others
class Monster : public FlightEntity
{
public:

	float health = 1.0f;

	TCODPath* current_path;

	bool just_started;

	float x_target, y_target;

	// Map coordinates, moves through the center
	// of tiles and never diagonally to avoid going into walls
	void path_to(int x, int y);

	virtual void update(float dt) override;

	virtual EntityType get_type() = 0;

	virtual int get_symbol() = 0;

	std::pair<int, int> get_map_position();

	virtual float get_speed() = 0;

	void step();

	// Use only within a tile, it does no pathing and
	// will happily go through walls!
	void move_to(float x, float y);
	bool in_destination();

	// Returns distance to point
	float go_to(float x, float y);

	// Moves randomly in points within radius of a given point
	void idle(int radius, int cx, int cy);

	float distance_to(float x, float y);

	float distance_to_player();

	std::pair<float, float> get_player_pos();

	float get_player_noise();


	Vehicle* get_player();

	Monster()
	{
		current_path = nullptr;
		x_target = -1.0f;
		y_target = -1.0f;
	}


	virtual bool is_alive() override
	{
		return health > 0.0f;
	}

	virtual void damage(float power) override
	{
		health -= power;
	}

};


class EntityCrawler : public Monster
{
public:

	SoLoud::Wav attack;
	SoLoud::Wav impact;
	SoLoud::Wav persecute;

	float velocity;

	bool persecuting;

	float speed_penalty;

	int c_x = -1, c_y = -1;

	virtual void update(float dt) override;

	virtual float get_speed_penalty_rate()
	{
		return 0.1f;
	}

	virtual bool autofind()
	{
		return false;
	}

	virtual void do_damage();


	virtual std::string get_name() override
	{
		return "Crawler";
	}

	virtual EntityType get_type() override
	{
		return E_CRAWLER;
	}

	virtual int get_symbol() override
	{
		return 157;
	}

	virtual float get_speed() override
	{
		return velocity;
	}

	EntityCrawler()
	{
		persecuting = false;
		speed_penalty = 0.0f;
		attack.load("crawler/attack.wav");
		impact.load("collide2.wav");
		persecute.load("crawler/idle.wav");
		health = 1.5f;
	};

};

class EntityBiter : public EntityCrawler
{
public:

	virtual std::string get_name() override
	{
		return "Biter";
	}

	virtual EntityType get_type() override
	{
		return E_BITER;
	}

	virtual int get_symbol() override
	{
		return 224;
	}

	virtual float get_speed_penalty_rate()
	{
		return 0.05f;
	}

	virtual bool autofind()
	{
		return true;
	}

	virtual void do_damage() override;

	EntityBiter()
	{
		health = 0.5f;
		attack.load("biter/attack.wav");
		persecute.load("biter/idle.wav");
	}
};

// TODO: Custom sounds to the BOMB
class EntityBomb : public EntityCrawler
{
public:

	virtual std::string get_name() override
	{
		return "Bomb";
	}

	virtual EntityType get_type() override
	{
		return E_BOMB;
	}

	virtual int get_symbol() override
	{
		return 155;
	}

	virtual float get_speed_penalty_rate()
	{
		return 0.05f;
	}

	virtual bool autofind()
	{
		return true;
	}

	virtual void do_damage() override;

	EntityBomb()
	{
		health = 0.5f;
		attack.load("biter/attack.wav");
		persecute.load("biter/idle.wav");
	}
};

// The worm moves randomly across the whole map
// and attacks, it's strong
class EntityWorm : public Monster
{
public: 

	float velocity;
	bool persecuting;
	bool dive;

	SoLoud::Wav dive_sound;
	SoLoud::Wav attack_sound;

	float rand_timer = 0.0f;

	virtual std::string get_name() override
	{
		return "Worm";
	}

	virtual int get_symbol()
	{
		return 126;
	}
	virtual EntityType get_type() override
	{
		return E_WORM;
	}

	virtual float get_speed() override
	{
		return velocity;
	}

	virtual void update(float dt) override;

	EntityWorm()
	{
		health = 2.0f;
		attack_sound.load("worm/attack.wav");
		dive_sound.load("worm/dive.wav");
		rand_timer = 0.0f;
	}

};

// Guardians refuse to go very far away from their nest
class EntityGuardian : public Monster
{
public:

	float velocity;
	float move_timer;
	float spawn_timer;

	float c_x, c_y;

	virtual void update(float dt) override;

	virtual std::string get_name() override
	{
		return "Guardian";
	}

	virtual EntityType get_type() override
	{
		return E_GUARDIAN;
	}

	virtual int get_symbol() override
	{
		return 236;
	}

	virtual float get_speed() override
	{
		return velocity;
	}

	EntityGuardian()
	{
		health = 4.0f;
		c_x = -1.0f;
		c_y = -1.0f;
		move_timer = 0.0f;
	}
};

// Serpents move across the map
class EntitySerpent : public Monster
{
public:

	float velocity;
	float spawn_timer;
	float engage_timer;
	float fight_timer;
	bool engaging;

	virtual void update(float dt) override;


	virtual std::string get_name() override
	{
		return "Giant Serpent";
	}

	virtual EntityType get_type() override
	{
		return E_SERPENT;
	}

	virtual int get_symbol() override
	{
		return 21;
	}

	virtual float get_speed() override
	{
		return velocity;
	}

	virtual void damage(float pow) override 
	{
		Monster::damage(pow);

		if (!engaging)
		{
			engaging = true;
			engage_timer = 60.0f;
		}
	
	}

	EntitySerpent()
	{
		health = 10.0f;
		engaging = false;
		spawn_timer = 0.0f;
		engage_timer = -1000.0f;
		fight_timer = 0.0f;
	}

};