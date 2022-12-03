#include "Component.h"
#include "Globals.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "ComponentCamera.h"

GameObject::GameObject(const char* name) : name(name), tag("Untagged"), layer("0: Default")
{
	
	transform = static_cast<Transform*>(AddComponent(ComponentType::TRANSFORM));
}

GameObject::~GameObject()
{
	
	RELEASE_VECTOR(childrens, childrens.size());
	childrens.clear();

	RELEASE_VECTOR(components, components.size());
	components.clear();

	if (pendingToDelete) this->GetParent()->EraseChildren(indexList);

	transform = nullptr;
	parent = nullptr;
}

void GameObject::Enable()
{
	active = true;
	if (parent != nullptr)
		parent->Enable();
}

void GameObject::AttachChild(GameObject* child)
{

	child->parent = this;
	childrens.push_back(child);
	child->transform->NewAttachment();
	child->transform->UpdateTransform();
}

void GameObject::Update()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* newComponent = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		newComponent = new Transform(this);
		break;
	case ComponentType::MESH_RENDERER:
		newComponent = new MeshRenderer(this);
		break;
	case ComponentType::MATERIAL:
		newComponent = new Material(this);
		break;
	case ComponentType::CAMERA:
		newComponent = new ComponentCamera(this);
		break;
	}
	newComponent->SetType(type);
	components.push_back(newComponent);

	return newComponent;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return components[i];
	}

	return nullptr;
}