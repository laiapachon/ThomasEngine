#pragma once

#include<vector>
#include<string>

class Component;
enum class ComponentType;
class Transform;

class GameObject
{
public:
	GameObject(const char* name);
	virtual ~GameObject();

	void Update();
	Component* AddComponent(ComponentType type);
	Component* GetComponent(ComponentType type);

	bool isActive() { return active; };
	void Enable();
	void Disable() { active = false; };

	bool IsRoot() { return (parent == nullptr) ? true : false; };
	void Destroy() { toDelete = true;};
	bool GetToDelete() { return toDelete; };

	std::vector<GameObject*> GetChildrens() { return children; };
	std::vector<Component*> GetComponents() { return components; };

	GameObject* parent;
	Transform* transform;
	bool active = true;
	std::string name;
	bool isStatic = false;

private:
	bool showChildren = false;
	bool toDelete = false;

	std::vector<GameObject*> children;
	std::vector<Component*> components;
};