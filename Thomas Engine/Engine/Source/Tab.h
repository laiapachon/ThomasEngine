#pragma once
#include <string>
#include "imgui/imgui.h"

#define LOG_MAX_LENGHT 100

class Tab
{
public:
	Tab() {};
	virtual ~Tab() {};

	virtual void Draw() = 0;

	std::string name = "";
	bool active = true;
	int shortcut;
};