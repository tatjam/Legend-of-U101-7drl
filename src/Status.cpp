#include "Status.h"



void Status::draw(int x0, int y0, int x1, int y1)
{
	int id = 0;
	for (int y = y1 - 1; y > y0; y--)
	{
		int i = strings.size() - id - 1;
		if (i >= 0 && strings.size() > 0)
		{
			TCODConsole::root->printf(x0, y, strings[i].c_str());
		}

		id++;
	}
}

Status::Status()
{
}


Status::~Status()
{
}
