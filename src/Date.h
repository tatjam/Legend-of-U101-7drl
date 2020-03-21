#pragma once

struct Date
{
	int day;
	int hour;
	int minute;
	int second;

	int to_seconds()
	{
		return second + minute * 60 + hour * 60 * 60 + day * 24 * 60 * 60;
	}

	void from_seconds(int s)
	{
		day = s / (24 * 60 * 60);
		s = s % (24 * 60 * 60);
		hour = s / (60 * 60);
		s = s % (60 * 60);
		minute = s / 60;
		s = s % 60;
		second = s;
	}

	std::string format()
	{
		std::string str;
		str += std::to_string(day);
		str += ':';
		str += std::to_string(hour);
		str += ':';
		str += std::to_string(minute);
		str += ':';
		str += std::to_string(second);

		return str;
	}

};