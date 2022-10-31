#include "Application.h"
#include "Scene.h"
#include "Globals.h"

#include "GameObject.h"

#include "Input.h"
#include "Editor.h"
#include "FileSystem.h"
#include "Inspector.h"

Scene::Scene(Application* app, bool start_enabled) : Module(app, start_enabled), root(nullptr)
{
	name = "scene";
}

Scene::~Scene()
{
}

bool Scene::Init()
{
	root = CreateGameObject("Root", nullptr);

	return true;
}

bool Scene::Start()
{
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
	//Delete all gameObjects
	delete root;

	return true;
}

GameObject* Scene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* obj = new GameObject(name);

	if (parent != nullptr)
		parent->GetChildrens().push_back(obj);

	return obj;
}

void Scene::Destroy(GameObject* obj)
{
	dynamic_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected = nullptr;

	for (std::vector<GameObject*>::iterator i = obj->parent->GetChildrens().begin(); i != obj->parent->GetChildrens().end(); ++i)
	{
		if (*i._Ptr == obj)
		{
			obj->parent->GetChildrens().erase(i);
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
	if (parent->GetToDelete())
	{
		destroyList.push_back(parent);
		return;
	}

	if (parent->isActive())
	{
		parent->Update();

		for (size_t i = 0; i < parent->GetChildrens().size(); i++)
		{
			RecursiveUpdate(parent->GetChildrens()[i]);
		}
	}
}