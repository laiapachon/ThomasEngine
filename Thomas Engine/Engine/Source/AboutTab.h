#pragma once
#include "Tab.h"
#include "Glew/include/glew.h"

class AboutTab : public Tab
{
public:
	AboutTab();
	~AboutTab() {};

	void Draw() override;
	void PrintLicense();
	void LogVersionDependences();

	void InitVersions();

private:
	std::string SDLVersion;
	std::string physVersion;
	std::string assimpVersion;

	const GLubyte* OGLVersion;
	const GLubyte* GlewVersion;
	const GLubyte* GLSLVersion;

	const char* mathGeoLib;
	const char* imGuiVersion;
	const char* parsonVersion;
	const char* deviLVersion;
};