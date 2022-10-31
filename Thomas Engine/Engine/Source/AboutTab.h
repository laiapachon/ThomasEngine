#pragma once
#include "Tab.h"

class AboutTab : public Tab
{
public:
	AboutTab();
	~AboutTab() {};

	void Draw() override;
	void PrintLicense();

	std::string SDLVersion;
	std::string physVersion;
	std::string assimpVersion;
};

