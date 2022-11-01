#include "Application.h"
#include "Scene.h"
#include "Globals.h"

//Components
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"

//Modules
#include "Input.h"
#include "Editor.h"
#include "ResourceManager.h"

#include "GameObject.h"
#include "Inspector.h"
#include "ResourceMesh.h"

#include"MathGeoLib/include/Math/Quat.h"

Scene::Scene(Application* app, bool start_enabled) : Module(app, start_enabled), root(nullptr)
{
	name = "scene";
}

Scene::~Scene()
{
}

bool Scene::Init()
{
	LOG(LogType::L_NORMAL, "Loading Scene");
	root = CreateGameObject("Root", nullptr);

	return true;
}

bool Scene::Start()
{
	app->resourceManager->ImportFile("BakerHouse.fbx");

	Transform* transformChimney = root->GetChildren()[0]->GetChildren()[0]->transform;
	Transform* transformBakerhouse = root->GetChildren()[0]->GetChildren()[1]->transform;
	Transform* parentTransform = root->GetChildren()[0]->transform;
	

	float3 size(1, 1, 1);
	Quat rotationQuat(0, 0, 0, 1);

	transformChimney->SetTransformMatrix(transformChimney->GetPosition(), rotationQuat, size, parentTransform);
	transformBakerhouse->SetTransformMatrix(transformBakerhouse->GetPosition(), rotationQuat, size, parentTransform);

	return true;
}

update_status Scene::PreUpdate(float dt)
{
	if (destroyList.size() > 0) {
		for (size_t i = 0; i < destroyList.size(); ++i)
		{
			Destroy(destroyList[i]);
		}
		destroyList.clear();
	}
	return update_status::UPDATE_CONTINUE;
}

update_status Scene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && app->editor->GetGameObjectSelected() != nullptr)
		app->editor->GetGameObjectSelected()->Destroy();

	UpdateGameObjects();

	return update_status::UPDATE_CONTINUE;
}

bool Scene::CleanUp()
{
	LOG(LogType::L_NORMAL, "Freeing Level Manager");
	//Delete all gameObjects
	delete root;

	return true;
}

GameObject* Scene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* obj = new GameObject(name);

	if (parent != nullptr)
		parent->AddChildren(obj);

	return obj;
}

GameObject* Scene::CreatePrimitive(const char* name, Mesh* mesh)
{
	GameObject* primitive = new GameObject(name);
	primitive->SetParent(root);

	MeshRenderer* meshRenderer;
	meshRenderer = dynamic_cast<MeshRenderer*>(primitive->AddComponent(ComponentType::MESHRENDERER));
	meshRenderer->mesh = mesh;

	root->AddChildren(primitive);

	return nullptr;
}

void Scene::Destroy(GameObject* obj)
{
	dynamic_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected = nullptr;

	for (std::vector<GameObject*>::iterator i = obj->GetParent()->GetChildren().begin(); i != obj->GetParent()->GetChildren().end(); ++i)
	{
		if (*i._Ptr == obj)
		{
			obj->GetParent()->GetChildren().erase(i);
			break;
		}
	}

	delete obj;
	obj = nullptr;
}

void Scene::UpdateGameObjects()
{
	RecursiveUpdate(root);
}

void Scene::RecursiveUpdate(GameObject* parent)
{
	if (parent->GetPendingToDelete())
	{
		destroyList.push_back(parent);
		return;
	}

	if (parent->active)
	{
		parent->Update();

		for (size_t i = 0; i < parent->GetChildren().size(); i++)
		{
			RecursiveUpdate(parent->GetChildren()[i]);
		}
	}
}