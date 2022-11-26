#pragma once

#include "Tab.h"

class SceneTab : public Tab
{
public:
	SceneTab();
	virtual ~SceneTab() {};

	void Draw() override;
	ImVec2 lastViewportSize;
};