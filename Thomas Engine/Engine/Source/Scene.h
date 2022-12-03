#pragma once
#include "Module.h"
#include "Transform.h"
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
	GameObject* CreateNewCamera();
	GameObject* CreateNewGameObject(const char* name);
	GameObject* CreateChildGameObject(const char* name, GameObject* parent);
	GameObject* CreateParentGameObject(const char* name, GameObject* child);
	GameObject* CreatePrimitive(const char* name, Mesh* mesh);

	void SaveSceneRequest() { saveSceneRequest = true; }
	void LoadSceneRequest() { loadSceneRequest = true; }

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
	GameObject* LoadGameObject(JsonParser root, GameObject* father = nullptr);
	void LoadComponents(JsonParser& parent, std::string& num, GameObject*& gamObj);
	float4x4 strMatrixToF4x4(const char* convert);

	JsonParser jsonFile;
	JSON_Value* rootFile;
	JsonParser rootGameObject;

	bool saveSceneRequest = false;
	bool loadSceneRequest = false;
};