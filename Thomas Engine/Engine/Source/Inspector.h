#pragma once
#include "Tab.h"
#include <string>
#include <list>

class GameObject;

class Inspector : public Tab
{
public:
	Inspector();
	virtual ~Inspector();

	void Draw() override;

	GameObject* gameObjectSelected;
};