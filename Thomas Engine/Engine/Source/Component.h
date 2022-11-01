#pragma once

#include "GameObject.h"
enum class ComponentType
{
	UNKNOWN,
	TRANSFORM,
	MESHRENDERER,
	MATERIAL,
};

class Component
{
public:
	Component(GameObject* obj) { owner = obj; };
	virtual ~Component() {};

	void Enable() { active = true; };
	void Disable() { active = false; };

	virtual void Update() {};
	virtual void OnEditor() {};

	ComponentType GetType() { return type; };
	void SetType(ComponentType type) { this->type = type; };
	
	GameObject* GetOwner() { return owner; };
private:
	bool active = true;

	ComponentType type = ComponentType::UNKNOWN;
	GameObject* owner;

};