#pragma once
#include "Module.h"

#include<vector>

class GameObject;
class Mesh;

class Scene : public Module
{
public:
	Scene(Application* app, bool start_enabled = true);
	virtual ~Scene();

	bool Init() override;
	bool Start() override;

	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;

	bool CleanUp() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	GameObject* CreatePrimitive(const char* name, Mesh* mesh);

	GameObject* root;

	std::vector<GameObject*> destroyList;

private:
	void Destroy(GameObject* obj);

	void UpdateGameObjects();
	void RecursiveUpdate(GameObject* parent);
};