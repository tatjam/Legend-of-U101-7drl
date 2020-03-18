#pragma once

#include "FlightEntity.h"

// If no entity is given (nullptr)
// the torpedo will go towards the player
class EntityTorpedo : public FlightEntity
{
private:

	FlightEntity* target;

public:

	float heading;
	float velocity;
	float maneouver;
	float safety;
	float lifetime;

	float lived;

	bool alive;

	virtual bool is_alive()
	{
		return alive;
	}

	
	virtual void damage(float power);


	EntityTorpedo()
	{
		target = nullptr;
		alive = true;
	}

	EntityTorpedo(FlightEntity* target);
	~EntityTorpedo();

	// Inherited via FlightEntity
	virtual void update(float dt) override;
	virtual EntityType get_type() override;
	virtual int get_symbol() override;
};

