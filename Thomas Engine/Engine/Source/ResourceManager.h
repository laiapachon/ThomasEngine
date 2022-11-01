#pragma once
#include "Module.h"
#include "Globals.h"
#include "Resource.h"

#include<map>

class Application;
class Texture;

class ResourceManager :public Module
{
public:
	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	bool Init();
	bool Start();
	bool CleanUp();

	void ImportFile(const char* assetsFile);

	Texture* logo;
};