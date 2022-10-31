#pragma once
#include <iostream>
#include <vector>
#include <list>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ResourceManager.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Input.h"
#include "Camera3D.h"
#include "Editor.h"
#include "Scene.h"

#include "JsonParser.h"

#define FILE_CONFIG	"../Output/Settings/config.json"
#include "Algorithm/Random/LCG.h"


class Application
{
public:
	ResourceManager* resourceManager;
	Window* window;
	Camera3D* camera;
	Input* input;
	Renderer3D* renderer3D;
	Scene* scene;
	Editor* editor;

	std::vector<Module*> listModules;
private:

	Timer	ms_timer;
	float	dt;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDt() const { return dt; }
	float GetFrameRate() const { return 1.f / dt; }

	void SaveConfig();
	void LoadConfig();

	inline void SaveConfigRequest() { saveRequested = true; }
	inline void LoadConfigRequest() { loadRequested = true; }

	inline const char* GetAppName() const { return TITLE; }
	inline const char* GetOrganizationName() const { return ORGANIZATION_NAME; }

	int GetNewUID() { return randomizer.Int(); }

	int maxFPS;
	int screenRefresh;


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	JsonParser jsonFile;

	bool saveRequested;
	bool loadRequested;

	LCG randomizer;
};
extern Application* app;