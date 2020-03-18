#pragma once

#include "FlightEntity.h"
#include "../../disembark/BuildingMap.h"


class Building : public FlightEntity
{
public:

	BuildingMap bmap;


	bool explored;

	bool is_explored()
	{
		return explored;
	}

	void explore()
	{
		explored = true;
	}

	BuildingMap& get_bmap()
	{
		return bmap;
	}

	Building(int x, int y, bool organic = false) : bmap(x, y, organic)
	{
		explored = false;
	}
};

// A building does very little
class EntityNest : public Building 
{
public:

	int attackers = 0;
	float timer;

	virtual void update(float dt) override;

	virtual EntityType get_type() override;

	virtual int get_symbol() override;


	EntityNest() : Building(80, 50, true)
	{
		attackers = g_random->getInt(1, 4);
		timer = 0.0f;
	}
};

class EntityStation : public Building 
{
public:
	virtual void update(float dt) override
	{

	}

	virtual EntityType get_type() override
	{
		return E_STATION;
	}

	virtual int get_symbol() override
	{
		return 234;
	}

	EntityStation() : Building(80, 50)
	{

	}

};

class EntityBase : public FlightEntity 
{
public:
	virtual void update(float dt) override
	{

	}

	virtual EntityType get_type() override
	{
		return E_BASE;
	}

	virtual int get_symbol() override
	{
		return 127;
	}


};

