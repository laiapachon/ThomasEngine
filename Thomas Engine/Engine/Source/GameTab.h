#pragma once
#include "Tab.h"

class Game : public Tab
{
public:
	Game();
	virtual ~Game() {};

	void Draw() override;
	ImVec2 lastViewportSize;
};