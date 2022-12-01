#pragma once
#include "Module.h"

#include<vector>

class GameObject;
class Mesh;
class ComponentCamera;

class Scene : public Module
{
public:
	Scene(Application* app, bool start_enabled = true);
	virtual ~Scene() {};

	bool Init() override;
	bool Start() override;

	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;

	bool CleanUp() override;
	void CreateNewCamera();
	GameObject* CreateNewGameObject(const char* name);
	GameObject* CreateChildGameObject(const char* name, GameObject* parent);
	GameObject* CreateParentGameObject(const char* name, GameObject* child);
	GameObject* CreatePrimitive(const char* name, Mesh* mesh);

	void SaveSceneRequest() { saveSceneRequest = true; }

	GameObject* root;
	ComponentCamera* mainCamera = nullptr;

	std::vector<GameObject*> destroyList;

private:
	void Destroy(GameObject* obj);

	void UpdateGameObjects();
	void RecursiveUpdate(GameObject* parent);

	bool SaveScene();
	void SaveGameObjects(GameObject* parent, JsonParser& rootFile);
	bool LoadScene();

	JsonParser jsonFile;
	JSON_Value* rootFile;

	bool saveSceneRequest = false;
};