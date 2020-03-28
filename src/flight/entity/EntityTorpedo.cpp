#include "EntityTorpedo.h"
#include "../FlightScene.h"

void EntityTorpedo::damage(float power)
{
	if (is_alive())
	{
		alive = false;

		// Explosion, damage all nearby entities and play sound
		// depending on player distance (we use explosion function)
		get_scene()->explosion(get_x(), get_y(), 0.5f);
	}
}

EntityTorpedo::EntityTorpedo(FlightEntity* target)
{
	this->target = target;
	alive = true;
}

EntityTorpedo::~EntityTorpedo()
{
}


inline float wrap_angle(float angle)
{
	float two_pi = 2.0f * PI;
	return angle - two_pi * floor(angle / two_pi);
}

void EntityTorpedo::update(float dt)
{
	lived += dt;
	safety -= dt;

	float tx, ty;
	if(target == nullptr)
	{
		tx = get_scene()->vehicle.x;
		ty = get_scene()->vehicle.y;
	}
	else
	{
		tx = target->get_x();
		ty = target->get_y();
	}

	float x = get_x();
	float y = get_y();

	float dx = tx - x;
	float dy = ty - y;

	float heading_wanted = atan2(dy, dx) + PI / 2.0f;

	float real_wanted = wrap_angle(heading_wanted);
	float real_vehicle = wrap_angle(heading);

	float dir = 1.0f;
	float diff = real_wanted - real_vehicle;

	if (fabs(diff) <= 0.005)
	{
		heading = heading_wanted;
	}
	else
	{
		if (fabs(diff) > PI)
		{
			dir = -1.0f;
		}

		if (diff < 0)
		{
			dir = -dir;
		}


		heading = real_vehicle + dir * dt * maneouver;
	}

	if (lived >= lifetime)
	{
		maneouver = 0.0f;
	}

	x += sin(heading) * dt * velocity;
	y -= cos(heading) * dt * velocity;

	set_position(x, y);

	float dist = sqrt(dx * dx + dy * dy);

	float thresold = 0.1f;
	if (abs(diff) > 0.3f)
	{
		thresold = 0.2f;
	}

	if (dist <= thresold && safety <= 0.0f && g_random->getFloat(0.0f, 1.0f) >= 0.8f)
	{
		damage(1.0f);
	}

	// Check collisions against walls
	if (get_map()->get_subtile(get_x(), get_y()) == WALL)
	{
		damage(1.0f);
	}


}

EntityType EntityTorpedo::get_type()
{
	return E_TORPEDO;
}

int EntityTorpedo::get_symbol()
{
	return 94;
}
