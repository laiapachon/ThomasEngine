#pragma once
#include "Module.h"
#include "Globals.h"
#include "Resource.h"

#include<map>

class Application;

class ResourceManager :public Module
{
public:
	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	bool Init();
	bool Start();
	bool CleanUp();

	int GenerateNewUID();
	int GetMetaUID(const char* metaFile) const;
	int ImportFile(const char* assetsFile, Resource::Type type);

	void UnloadResource(int uid);

	Resource* CreateNewResource(const char* assetsFile, uint uid, Resource::Type type);
	Resource* RequestResource(int uid, const char* libraryPath = nullptr);
	Resource* GetResourceFromUID(int uid);

	std::string GetMetaPath(const char* assetsFile);
	std::string GenLibraryPath(uint _uid, Resource::Type _type);
	Resource::Type GetTypeFromLibraryExtension(const char* libraryFile) const;

	void UpdateMeshesDisplay();

private:
	void ReleaseResource(int uid);

private:
	std::map<int, Resource*> resources;
};
