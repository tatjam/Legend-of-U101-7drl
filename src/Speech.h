#pragma once
#include <string>

class Speech
{
public:

	static std::string positive()
	{
		if (rand() % 2 == 0)
		{
			if (rand() % 2 == 0)
			{
				return "Affirmative, ";
			}
			else
			{
				return "Received, ";
			}
		}
		else
		{
			if (rand() % 2 == 0)
			{
				return "Yes sir, ";
			}
			else
			{
				return "Solid copy sir, ";
			}
		}
	}

	static std::string heading()
	{
		if (rand() % 2 == 0)
		{
			return "setting heading to the ";
		}
		else
		{
			return "changing heading to the ";
		}
	}

	static std::string heading_done()
	{
		if (rand() % 2 == 0)
		{
			return "Heading set, sir!";
		}
		else
		{
			return "Sir, heading is set";
		}
	}

	static std::string speed_done()
	{
		if (rand() % 2 == 0)
		{
			return "Velocity set, sir!";
		}
		else
		{
			return "Sir, we are at the target velocity";
		}
	}
};