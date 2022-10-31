#pragma once

#include "GameObject.h"
enum class ComponentType
{
	UNKNOW,
	TRANSFORM,
	MESHRENDERER,
	MATERIAL
};

class Component
{
public:
	Component(GameObject* obj) { owner = obj; };
	virtual ~Component() {};

	virtual void Enable() {};
	virtual void Disable() {};

	virtual void Update() {};
	virtual void OnEditor() {};

	ComponentType GetType() { return type; };
	void SetType(ComponentType type) { this->type = type; };

	GameObject* GetOwner() { return owner; };

private:
	ComponentType type = ComponentType::UNKNOW;
	bool active = true;
	GameObject* owner;
};