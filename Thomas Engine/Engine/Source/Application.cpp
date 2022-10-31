#include "Application.h"


using namespace std;

extern Application* app = nullptr;
Application::Application() : maxFPS(60)
{
	app = this;
	resourceManager = new ResourceManager(this);
	window = new Window(this);
	input = new Input(this);
	renderer3D = new Renderer3D(this);
	camera = new Camera3D(this);
	scene = new Scene(this);
	editor = new Editor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(resourceManager);
	AddModule(camera);
	AddModule(input);
	AddModule(scene);

	// Renderer last!
	AddModule(editor);
	AddModule(renderer3D);

	loadRequested = true;
	saveRequested = false;
}

Application::~Application()
{

	for (int i = listModules.size() - 1; i >= 0; --i)
	{
		delete listModules[i];
		listModules[i] = nullptr;
	}

	listModules.clear();
}

bool Application::Init()
{
	bool ret = true;

	{
		bool ret = true;

		JSON_Value* root = jsonFile.FileToValue(FILE_CONFIG);

		if (jsonFile.GetRootValue() == NULL)
		{
			LOG(LogType::L_NORMAL, "Couldn't load %s", FILE_CONFIG);
			ret = false;
		}

		JsonParser application = jsonFile.GetChild(root, "App");

		maxFPS = application.JsonValToNumber("FPS");
	}



	// Call Init() in all modules
	for (unsigned int i = 0; i < listModules.size() && ret == true; i++)
	{
		ret = listModules[i]->Init();
	}
	// After all Init calls we call Start() in all modules
	LOG(LogType::L_NORMAL, "-------------- Application Start --------------");

	// Call Init() in all modules
	for (unsigned int i = 0; i < listModules.size() && ret == true; i++)
	{
		ret = listModules[i]->Start();
	}

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (loadRequested) LoadConfig();
	if (saveRequested) SaveConfig();

	if (maxFPS == 0 || renderer3D->vsync || maxFPS > screenRefresh)
	{
		Uint32 last_frame_ms = ms_timer.Read();
		float wait_time = (1000.f / (float)screenRefresh) - (float)last_frame_ms;
		SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
	}
	else if (maxFPS > 0)
	{
		Uint32 last_frame_ms = ms_timer.Read();
		float wait_time = (1000.f / (float)maxFPS) - (float)last_frame_ms;
		SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();


	for (unsigned int i = 0; i < listModules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = listModules[i]->PreUpdate(dt);
	}
	for (unsigned int i = 0; i < listModules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = listModules[i]->Update(dt);
	}
	for (unsigned int i = 0; i < listModules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = listModules[i]->PostUpdate(dt);
	}

	FinishUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	// Cleanup

	for (int i = listModules.size() - 1; i >= 0 && ret == true; --i)
	{
		ret = listModules[i]->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	//listModules.add(mod);
	listModules.push_back(mod);
}
void Application::SaveConfig()
{
	LOG(LogType::L_NORMAL, "Saving configuration");

	JSON_Value* root = jsonFile.GetRootValue();

	JsonParser application = jsonFile.SetChild(root, "App");

	application.SetNewJsonNumber(application.ValueToObject(application.GetRootValue()), "FPS", maxFPS);


	// Call SaveConfig() in all modules
	std::vector<Module*>::iterator item;

	for (item = listModules.begin(); item != listModules.end(); ++item)
	{
		(*item)->SaveConfig(jsonFile.SetChild(root, (*item)->name));
	}

	jsonFile.SerializeFile(root, FILE_CONFIG);
	saveRequested = false;
}


void Application::LoadConfig()
{
	LOG(LogType::L_NORMAL, "Loading configurations");

	JSON_Value* root = jsonFile.GetRootValue();

	JsonParser application = jsonFile.GetChild(root, "App");

	maxFPS = application.JsonValToNumber("FPS");


	std::vector<Module*>::iterator item;

	for (item = listModules.begin(); item != listModules.end(); ++item)
	{
		(*item)->LoadConfig(jsonFile.GetChild(root, (*item)->name));
	}

	loadRequested = false;
}