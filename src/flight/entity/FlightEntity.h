#pragma once

#include "../../defines.h"
#include <tuple>

class FlightMap;
class FlightScene;

enum EntityType
{
	E_NEST,
	E_STATION,
	E_BASE,

	// Monsters

	// The crawler is weak but moves pretty fast
	// It can only live in high oxygen tiles, and cannot
	// move out of them
	// Bad ears
	E_CRAWLER,
	// Opposite of the crawler, strong as hell but slow
	// Lives anywhere and has good ears. Your usual enemy
	E_WORM,

	// Medium speed, medium strength. Usually stays near nests
	// Spawns bombs and biters
	E_GUARDIAN,

	// Slow and very strong
	// Can spawn bombs, biters and crawlers
	// Very very rare
	E_SERPENT,
	// Fast moving, very weak. Explodes once near the submarine or destroyed
	E_BOMB,
	// Fast moving, very weak. Gets close to the submarine, and tries to cause
	// damage
	E_BITER,
	// Allied (or not) submarines. Spawn at bases and move to random points in the map
	E_SUBMARINE,

	// Torpedoes must be given a target by their creator, which they will try and hit
	// then they will explode
	E_TORPEDO,


};

class FlightEntity
{
private:

	FlightMap* map;
	FlightScene* scene;

	float x, y;
	bool visible;

	bool identified;

public:



	virtual void update(float dt) = 0;
	virtual EntityType get_type() = 0;
	virtual int get_symbol() = 0;

	virtual bool is_alive() 
	{
		return true;
	}

	// By default is does nothing
	virtual void damage(float power)
	{

	}

	std::pair<float, float> get_position() 
	{
		return std::make_pair(x, y);
	}

	void set_position(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	float get_x()
	{
		return this->x;
	}

	float get_y()
	{
		return this->y;
	}

	FlightMap* get_map()
	{
		return map;
	}

	FlightScene* get_scene()
	{
		return scene;
	}

	void init(FlightMap* map, FlightScene* scene)
	{
		this->map = map;
		this->scene = scene;
	}

	bool is_visible()
	{
		return visible;
	}

	void set_visible(bool v)
	{
		visible = v;

		if (v == false)
		{
			identified = false;
		}
	}

	void identify()
	{
		identified = true;
	}

	bool is_identified()
	{
		return identified;
	}

	virtual std::string get_name()
	{
		return "Forgot name";
	}

};