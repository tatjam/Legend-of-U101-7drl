#include "FlightCrew.h"
#include "workbench/Workbench.h"



FlightCrew::FlightCrew(Crewmember* c)
{
	this->gc = c;
	repair_order = false;
	path = nullptr;
	path_t = 0.0f;
	// Procedural

}

bool FlightCrew::can_work_in(Workbench * bench)
{
	/*for (size_t i = 0; i < assigned.size(); i++)
	{
		if (bench == assigned[i])
		{
			return true;
		}
	}
	
	return false;
	*/

	return true;
}

void FlightCrew::path_to(int dx, int dy, TCODMap& map)
{
	if (path != nullptr)
	{
		delete path;
		path = nullptr;
	}
	path = new TCODPath(&map);
	if (!path->compute(x, y, dx, dy))
	{
		delete path;
		path = nullptr;
	}
}
