#pragma once

#include <string>
#include <vector>
#include "../../Date.h"
#include "libtcod.h"

#include "Workbench.h"
#include "../../Drawing.h"



class Radio : public Workbench
{
private:
	float dt;

	bool dirty = false;


public:

	int msg;

	std::vector<std::string> messages;

	TCODConsole console;

	SoLoud::Speech voice_in;
	SoLoud::Wav distort;

	SoLoud::handle handle;
	SoLoud::handle distort_handle;

	Radio();
	~Radio();

	void say_message(std::string& msg);

	void push_message(std::string msg);

	// Inherited via Workbench
	virtual bool update(float dt) override;
	virtual void draw(int rx, int ry) override;
	virtual TCODConsole* get_console() override
	{
		return &console;
	}

	virtual std::string get_name() override
	{
		return "Radio";
	}
};

