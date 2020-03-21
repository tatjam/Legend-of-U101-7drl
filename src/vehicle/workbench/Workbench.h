#pragma once


#include <string>
#include <array>
#include <vector>
#include <stack>
#include "../FlightCrew.h"

class Vehicle;

enum Window
{
	NONE,
	SONAR,
	RADIO,
	MANEOUVER,
	PERISCOPE,
	MACHINES,
	BATTERY,
	TARGETING,
};

class Workbench
{
private:

	Vehicle* vehicle;

	bool is_open_v;

	std::stack<std::string> orders;


public:
	
	std::vector<FlightCrew>* crew;


	// Chair x and y
	int cx, cy;
	// Includes the chair
	std::vector<std::pair<int, int>> tiles;

	// Return true if we closed
	virtual bool update(float dt) = 0;
	virtual void draw(int rx, int ry) = 0;
	virtual TCODConsole* get_console() = 0;

	std::pair<int, int> get_size()
	{
		return std::make_pair(get_console()->getWidth(), get_console()->getHeight());
	}

	Vehicle* get_vehicle()
	{
		return vehicle;
	}

	void set_vehicle(Vehicle* v)
	{
		this->vehicle = v;
	}

	void open()
	{
		is_open_v = true;
	}

	void close()
	{
		is_open_v = false;
	}

	bool is_open()
	{
		return is_open_v;
	}

	virtual std::string get_name() = 0;

	FlightCrew* get_crewman()
	{
		for (FlightCrew& cm : *crew)
		{
			if (cm.can_work_in(this) && cm.x == cx && cm.y == cy)
			{
				return &cm;
			}
		}

		return nullptr;
	}

	bool is_crewed()
	{
		for (FlightCrew& cm : *crew)
		{
			if (cm.can_work_in(this) && cm.x == cx && cm.y == cy)
			{
				return true;
			}
		}

		return false;
	}

	// Order system, TODO
	/*void request_crew()
	{
		if (is_crewed())
		{
			return;
		}

		// Find first priority members
		for (Crewmember& cm : *crew)
		{
			if (cm.assigned.size() != 0)
			{
				if (cm.assigned[0] == this && cm.x == cx && cm.y == cy)
				{

				}
			}
		}

		// Find any crew with this workbench assigned
		for (Crewmember& cm : *crew)
		{
			if (cm.can_work_in(this) && cm.x == cx && cm.y == cy)
			{

			}
		}
	}

	std::string pop_order()
	{
		if (orders.empty())
		{
			return "";
		}

		std::string o = orders.top();
		orders.pop();
		return o;
	}

	void add_order(std::string name, bool req_crew = true)
	{
		orders.push(name);
		if (req_crew) 
		{
			request_crew();
		}
	}*/


};

