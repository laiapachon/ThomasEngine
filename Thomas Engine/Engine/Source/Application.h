#pragma once
#include <iostream>
#include <vector>
#include <list>

#include "Timer.h"
#include "JsonParser.h"

#define FILE_CONFIG	"../Output/Settings/config.json"
#define SCENE_FILE "../Output/Settings/scene.json"

#include "Algorithm/Random/LCG.h"

class Window;
class Renderer3D;
class ResourceManager;
class Scene;
class Game;
class Module;
class Input;
class Camera3D;
class Editor;

class Application
{
public:
	ResourceManager* resourceManager;
	Window* window;
	Camera3D* camera;
	Input* input;
	Renderer3D* renderer3D;
	Scene* scene;
	Game* game;
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