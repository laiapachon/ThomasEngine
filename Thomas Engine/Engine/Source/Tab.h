#pragma once
#include <string>

class Tab
{

public:
	Tab();
	virtual ~Tab();

	virtual void Draw() = 0;

	std::string name;
	bool active;
	int shortcut;
};