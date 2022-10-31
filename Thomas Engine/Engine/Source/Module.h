#pragma once
#include "JsonParser.h"

class Application;

class Module
{

public:
	Application* App;

	Module(Application* parent, bool startEnabled = true) : App(parent), enabled(startEnabled), name("") {}

	virtual ~Module(){}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual void OnGUI(){}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual bool LoadConfig(JsonParser& node)
	{
		return true;
	}

	virtual bool SaveConfig(JsonParser& node) const
	{
		return true;
	}
private :
	bool enabled = true;

public:
	const char* name;
};