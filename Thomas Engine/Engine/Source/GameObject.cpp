#include "GameObject.h"
#include "Component.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"

GameObject::GameObject(const char* name) : parent(nullptr), name(name)
{
	// Each GameObject must have a transform component, that's why we add it when creating it
	transform = dynamic_cast<Transform*>(AddComponent(ComponentType::TRANSFORM));
}

GameObject::~GameObject()
{
	// Delete all components
	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
	components.clear();

	// Delete all childrens
	for (size_t i = 0; i < childrens.size(); i++)
	{
		delete childrens[i];
		childrens[i] = nullptr;
	}
	childrens.clear();
}

void GameObject::Enable()
{
	active = true;

	// Recursive function, call all fathers
	// Because for the children to be active the father must also be 
	if (parent != nullptr)
		parent->Enable();
}

void GameObject::Update()
{
	// Call all components Updates
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}
// Add component by Type
Component* GameObject::AddComponent(ComponentType type)
{
	// Can't add a component without type
	assert(type != ComponentType::UNKNOWN, "Can't create a UNKNOW component");
	Component* newComponent = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		newComponent = new Transform(this);
		break;
	case ComponentType::MESHRENDERER:
		newComponent = new MeshRenderer(this);
		break;
	case ComponentType::MATERIAL:
		newComponent = new Material(this);
		break;
	}

	if (newComponent != nullptr)
	{
		newComponent->SetType(type);
		components.push_back(newComponent);
	}

	return newComponent;
}
// Get component by Type, because an object can only have 1 component of 1 type
Component* GameObject::GetComponent(ComponentType type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return components[i];
	}

	return nullptr;
}