#pragma once
#include "Module.h"
#include "Globals.h"
#include "Resource.h"

#include<map>

class Application;
class Texture;
class Shader;

class ResourceManager :public Module
{
public:
	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager() {};

	bool Init()override;
	bool Start()override;
	bool CleanUp()override;

	bool GetOverwritting() { return overwritting; };
	void Overwrite();
	void DrawOverwriteTab();
	void NewCounterFile();

	void ImportFile(const char* assetsFile);

	Texture* logo;
	Texture* backButton;
	Texture* addButton;

	Shader* GetShader(const std::string& name);
	std::map<std::string, Shader*> GetShadersMap();

private:
	
	std::map<std::string, Shader*> shadersMap;

	Resource* ovResource;
	bool overwritting = false;
	const char* ovPath;
	char* ovBuffer;
	uint ovSize;
};
