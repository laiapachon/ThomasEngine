#pragma once
#include "Tab.h"
#include <string>
#include <vector>

class GameObject;

class Inspector : public Tab
{
public:
	Inspector();
	virtual ~Inspector();

	void Draw() override;

	GameObject* gameObjectSelected;

	std::vector<std::string> tags;
	std::vector<std::string> layers;

};