#include "Application.h"
#include "Scene.h"
#include "Globals.h"

//Components
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentCamera.h"

//Modules
#include "Input.h"
#include "Editor.h"
#include "ResourceManager.h"
#include "Camera3D.h"

#include "GameObject.h"
#include "Inspector.h"
#include "ResourceMesh.h"

#include"MathGeoLib/include/Math/Quat.h"

Scene::Scene(Application* app, bool start_enabled) : Module(app, start_enabled), root(nullptr)
{
	name = "scene";
}

bool Scene::Init()
{
	LOG(LogType::L_NORMAL, "Creating Scene");

	root = new GameObject("Root");

	GameObject* sceneCamera = CreateNewGameObject("MainCamera");
	sceneCamera->AddComponent(ComponentType::CAMERA);
	mainCamera = static_cast<ComponentCamera*>(sceneCamera->GetComponent(ComponentType::CAMERA));
	mainCamera->SetIsMainCamera(true);

	jsonFile.FileToValue(SCENE_CONFIG);

	return true;
}

bool Scene::Start()
{
	app->resourceManager->ImportFile("BakerHouse.fbx");
	Transform* transformChimney = root->GetChildrens()[1]->GetChildrens()[0]->transform;
	Transform* transformBakerhouse = root->GetChildrens()[1]->GetChildrens()[1]->transform;
	Transform* parentTransform = root->GetChildrens()[1]->transform;

	float3 size(1, 1, 1);
	Quat rotationQuat(0, 0, 0, 1);

	transformChimney->SetTransformMatrix(transformChimney->GetPosition(), rotationQuat, size, parentTransform);
	transformBakerhouse->SetTransformMatrix(transformBakerhouse->GetPosition(), rotationQuat, size, parentTransform);

	root->EraseChildren(root->FindChildren(root->GetChildrens()[0]));
	root->AttachChild(mainCamera->GetOwner());
	Transform* transformCamera = static_cast<Transform*>(mainCamera->GetOwner()->GetComponent(ComponentType::TRANSFORM));
	transformCamera->SetPosition(float3(0, 1, -12));

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
	LOG(LogType::L_NO_PRINTABLE, "Freeing Level Manager");
	//Delete all gameObjects
	delete root;

	return true;
}

GameObject* Scene::CreateNewGameObject(const char* name)
{
	GameObject* obj = new GameObject(name);
	root->AttachChild(obj);

	return obj;
}

GameObject* Scene::CreateChildGameObject(const char* name, GameObject* parent)
{
	GameObject* obj = new GameObject(name);

	if (parent != nullptr)
		parent->AttachChild(obj);

	return obj;
}

GameObject* Scene::CreateParentGameObject(const char* name, GameObject* child)
{
	GameObject* obj = new GameObject(name);

	child->GetParent()->AttachChild(obj);
	child->GetParent()->EraseChildren(child->GetParent()->FindChildren(child));
	obj->AttachChild(child);

	return obj;
}

void Scene::CreateNewCamera()
{
	GameObject* camera = CreateNewGameObject("Camera");
	camera->AddComponent(ComponentType::CAMERA);
	Transform* transformCamera = static_cast<Transform*>(camera->GetComponent(ComponentType::TRANSFORM));
	transformCamera->SetPosition(float3(0, 3.5f, -12));
	transformCamera->SetEulerRotaion(float3(6, 0, 0));
}


GameObject* Scene::CreatePrimitive(const char* name, Mesh* mesh)
{
	GameObject* primitive = new GameObject(name);
	primitive->SetParent(root);

	MeshRenderer* meshRenderer;
	meshRenderer = static_cast<MeshRenderer*>(primitive->AddComponent(ComponentType::MESH_RENDERER));
	meshRenderer->SetMesh(mesh);

	root->AddChildren(primitive);
	meshRenderer->SetBoundingBoxes(mesh);

	return nullptr;
}

void Scene::Destroy(GameObject* obj)
{
	app->editor->SetGameObjectSelected(nullptr);
	for (std::vector<GameObject*>::iterator i = obj->GetParent()->GetBeginChildren(); i != obj->GetParent()->GetEndChildren(); ++i)
	{
		if (*i._Ptr == obj)
		{
			obj->SetIndex(i);
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

		for (size_t i = 0; i < parent->GetChildrens().size(); i++)
		{
			RecursiveUpdate(parent->GetChildrens()[i]);
		}
	}
}

bool Scene::SaveScene()
{
	LOG(LogType::L_NORMAL, "Saving scene");

	rootFile = jsonFile.GetRootValue();

	JsonParser scene = jsonFile.SetChild(rootFile, "GameObjects");

	SaveGameObjects(root, scene.SetChild(scene.GetRootValue(), root->name.c_str()));

	jsonFile.FileSerialization(rootFile, SCENE_CONFIG);
	saveSceneRequest = false;
	return true;
}

void Scene::SaveGameObjects(GameObject* parentGO, JsonParser& node)
{
	//node.SetChild(node.GetRootValue(), "Child");
	for (size_t i = 0; i < parentGO->GetChildrens().size(); i++)
	{
		SaveGameObjects(parentGO->GetChildrens()[i], node.SetChild(node.SetChild(node.GetRootValue(), "Child").GetRootValue(), parentGO->GetChildrens()[i]->name.c_str()));

		node.SetJBool(node.ValueToObject(node.GetRootValue()), "IsRoot", parentGO->IsRoot());

		node.SetJBool(node.ValueToObject(node.GetRootValue()), "active", parentGO->active);
		node.SetJBool(node.ValueToObject(node.GetRootValue()), "isStatic", parentGO->isStatic);
		node.SetJBool(node.ValueToObject(node.GetRootValue()), "isSelected", parentGO->isSelected);

		node.SetJString(node.ValueToObject(node.GetRootValue()), "name", parentGO->name.c_str());
		node.SetJString(node.ValueToObject(node.GetRootValue()), "tag", parentGO->tag.c_str());
		node.SetJString(node.ValueToObject(node.GetRootValue()), "layer", parentGO->layer.c_str());

		node.SetJBool(node.ValueToObject(node.GetRootValue()), "showChildrens", parentGO->GetShowChildrens());
		node.SetJBool(node.ValueToObject(node.GetRootValue()), "pendingToDelete", parentGO->GetPendingToDelete());

		JsonParser& components = node.SetChild(node.GetRootValue(), "components");

		for (size_t i = 0; i < parentGO->GetCompomemts().size(); i++)
		{
			components.SetJBool(node.ValueToObject(node.GetRootValue()), "pendingToDelete", parentGO->GetPendingToDelete());
			parentGO->GetCompomemts().at(i)->GetType();
		}

	}
}

bool Scene::LoadScene()
{

	return true;
}