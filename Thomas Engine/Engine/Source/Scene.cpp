#include "Application.h"
#include "Scene.h"
#include "Globals.h"

#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentCamera.h"

#include "Input.h"
#include "Editor.h"
#include "ResourceManager.h"
#include "Camera3D.h"
#include "Material.h"
#include "Texture.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Inspector.h"
#include "ResourceMesh.h"
#include "Window.h"
#include "Renderer3D.h"

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

	jsonFile.FileToValue(SCENE_FILE);

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
	transformCamera->SetTransformMFromM(transformCamera->GetLocalTransform());

	app->resourceManager->ImportFile("street2.fbx");

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

GameObject* Scene::CreateNewCamera()
{
	GameObject* camera = CreateNewGameObject("Camera");
	camera->AddComponent(ComponentType::CAMERA);
	Transform* transformCamera = static_cast<Transform*>(camera->GetComponent(ComponentType::TRANSFORM));
	transformCamera->SetPosition(float3(0, 3.5f, -12));
	transformCamera->SetEulerRotaion(float3(6, 0, 0));
	return camera;
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

	if (!obj->IsRoot())	
	{
		for (std::vector<GameObject*>::iterator i = obj->GetParent()->GetBeginChildren(); i != obj->GetParent()->GetEndChildren(); ++i)
		{
			if (*i._Ptr == obj)
			{
				obj->SetIndex(i);
				break;
			}
		}
	}

	delete obj;
	obj = nullptr;
}

void Scene::UpdateGameObjects()
{
	RecursiveUpdate(root);
	if (saveSceneRequest)SaveScene();
	if (loadSceneRequest)LoadScene();
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

	jsonFile.FileSerialization(rootFile, SCENE_FILE);
	saveSceneRequest = false;
	return true;
}

void Scene::SaveGameObjects(GameObject* parentGO, JsonParser& node)
{
	//node.SetChild(node.GetRootValue(), "Child");
	std::string num, strTmp;
	JsonParser& child = node;
	Transform* transform;
	float4x4 localTransform, globalTransform;

	MeshRenderer* mesh;
	Material* material;
	ComponentCamera* camera;

	node.SetJString(node.ValueToObject(node.GetRootValue()), "name", parentGO->name.c_str());
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "IsRoot", parentGO->IsRoot());

	node.SetJString(node.ValueToObject(node.GetRootValue()), "tag", parentGO->tag.c_str());
	node.SetJString(node.ValueToObject(node.GetRootValue()), "layer", parentGO->layer.c_str());


	node.SetJBool(node.ValueToObject(node.GetRootValue()), "active", parentGO->active);
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "isStatic", parentGO->isStatic);
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "isSelected", parentGO->isSelected);


	node.SetJBool(node.ValueToObject(node.GetRootValue()), "showChildrens", parentGO->GetShowChildrens());
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "pendingToDelete", parentGO->GetPendingToDelete());

	//Create father Components
	JsonParser components = node.SetChild(node.GetRootValue(), "components");
	JsonParser tmp = node;
	for (size_t i = 0; i < parentGO->GetComponents().size(); i++)
	{
		// Create Child of component
		num = "Component " + std::to_string(i);
		tmp = components.SetChild(components.GetRootValue(), num.c_str());

		tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "Type", (int)parentGO->GetComponents().at(i)->GetType());
		tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "active", parentGO->GetComponents().at(i)->active);

		switch ((ComponentType)parentGO->GetComponents().at(i)->GetType())
		{
		case ComponentType::TRANSFORM:
			num = "";
			transform = static_cast<Transform*>(parentGO->GetComponent(ComponentType::TRANSFORM));
			localTransform = transform->GetLocalTransform();
			globalTransform = transform->GetGlobalTransform();
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
				{
					num += std::to_string(localTransform.At(i, j)), strTmp += std::to_string(globalTransform.At(i, j));
					strTmp += ",";
					num += ",";
				}

			tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "LocalTransform", num.c_str());
			tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "GlobalTransform", strTmp.c_str());

			break;

		case ComponentType::MESH_RENDERER:
			mesh = static_cast<MeshRenderer*>(parentGO->GetComponent(ComponentType::MESH_RENDERER));
			tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "Mesh", mesh->GetMesh()->GetAssetPath());
			tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "LibraryPath", mesh->GetMesh()->GetLibraryPath());
			break;

		case ComponentType::MATERIAL:
			material = static_cast<Material*>(parentGO->GetComponent(ComponentType::MATERIAL));
			tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "Material", material->texture->path.c_str());
			tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "LibraryPath", material->texture->GetLibraryPath());
			break;

		case ComponentType::CAMERA:
			camera = static_cast<ComponentCamera*>(parentGO->GetComponent(ComponentType::CAMERA));

			tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "isMainCamera", camera->GetIsMainCamera());
			tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "showFrustrum", camera->GetShowFrustrum());

			break;

		}
		parentGO->GetComponents().at(i)->GetType();
	}

	for (size_t i = 0; i <= parentGO->GetChildrens().size(); i++)
	{
		num = "Child " + std::to_string(i);
		if (parentGO->GetChildrens().size() > i)
			SaveGameObjects(parentGO->GetChildrens()[i], child.SetChild(child.GetRootValue(), num.c_str()));
	}
}

bool Scene::LoadScene()
{
	LOG(LogType::L_NORMAL, "Loading configurations");

	Destroy(root);

	rootFile = jsonFile.GetRootValue();

	rootGameObject = jsonFile.GetChild(rootFile, "GameObjects");
	root = LoadGameObject(rootGameObject.GetChild(rootGameObject.GetRootValue(), "Root"));

	loadSceneRequest = false;

	return true;
}

GameObject* Scene::LoadGameObject(JsonParser parent, GameObject* father)
{
	std::string num;
	std::string convert;
	std::string name = parent.JsonValToStr("name");

	GameObject* gamObj = new GameObject(name.c_str());

	LOG(LogType::L_NORMAL, (std::string("\n Loading ") + name).c_str());

	if (!parent.JsonValToBool("IsRoot"))gamObj->SetParent(father);

	gamObj->tag = parent.JsonValToStr("tag");
	gamObj->layer = parent.JsonValToStr("layer");
	gamObj->active = parent.JsonValToBool("active");
	gamObj->isStatic = parent.JsonValToBool("isStatic");
	gamObj->isSelected = parent.JsonValToBool("isSelected");
	gamObj->SetShowChildrens(parent.JsonValToBool("showChildrens"));
	gamObj->SetPendingToDelete(parent.JsonValToBool("pendingToDelete"));

	LoadComponents(parent, num, gamObj);
	int count = 0;
	num = "Child " + std::to_string(count);
	while (parent.ExistingChild(parent.GetRootValue(), num.c_str()))
	{
		gamObj->AttachChild(LoadGameObject(parent.GetChild(parent.GetRootValue(), num.c_str()), gamObj));
		++count;
		num = "Child " + std::to_string(count);
	}
	app->renderer3D->renderQueue.clear();

	return gamObj;
}

void Scene::LoadComponents(JsonParser& parent, std::string& num, GameObject*& gamObj)
{

	Transform* transform;
	MeshRenderer* meshRender;
	Mesh* mesh;
	Material* material;
	ComponentCamera* camera;

	LOG(LogType::L_NORMAL, "Loading Components \n");

	JsonParser components = parent.GetChild(parent.GetRootValue(), "components");
	JsonParser tmp = components;

	for (int i = 0; i < 4; i++)
	{
		num = "Component " + std::to_string(i);
		LOG(LogType::L_NORMAL, (std::string("Loading ") + num).c_str());

		if (components.ExistingChild(components.GetRootValue(), num.c_str()))
		{
			tmp = components.GetChild(components.GetRootValue(), num.c_str());
			switch ((ComponentType)(int)tmp.JsonValToNumber("Type"))
			{
			case ComponentType::TRANSFORM:
				gamObj->transform->SetGlobalTransform(strMatrixToF4x4(tmp.JsonValToStr("GlobalTransform")));
				if (!gamObj->IsRoot()) gamObj->transform->SetTransformMFromM(gamObj->transform->GetGlobalTransform());
				gamObj->transform->SetLocalTransform(strMatrixToF4x4(tmp.JsonValToStr("LocalTransform")));

				//gamObj->transform = transform;

				break;
			case ComponentType::MESH_RENDERER:
				gamObj->AddComponent(ComponentType::MESH_RENDERER);
				meshRender = static_cast<MeshRenderer*>(gamObj->GetComponent(ComponentType::MESH_RENDERER));
				if (meshRender != NULL)
				{
					mesh = new Mesh();
					mesh->SetLibraryPath(tmp.JsonValToStr("LibraryPath"));
					mesh->SetAssetsPath(tmp.JsonValToStr("Mesh"));
					meshRender->SetMesh(mesh);
					meshRender->SetOwner(gamObj);
				}
				break;
			case ComponentType::MATERIAL:
				gamObj->AddComponent(ComponentType::MATERIAL);
				material = static_cast<Material*>(gamObj->GetComponent(ComponentType::MATERIAL));
				material->active = tmp.JsonValToBool("active");
				material->texture = new Texture(tmp.JsonValToStr("Material"), gamObj->name);
				material->texture->SetLibraryPath(tmp.JsonValToStr("LibraryPath"));
				material->texture->path = material->texture->GetLibraryPath();
				//material->texture->SetAssetsPath();
				material->SetOwner(gamObj);
				material->texture->LoadToMemory();


				break;
			case ComponentType::CAMERA:

				gamObj->AddComponent(ComponentType::CAMERA);
				camera = static_cast<ComponentCamera*>(gamObj->GetComponent(ComponentType::CAMERA));
				mainCamera = camera;
				camera->ReGenerateFrameBuffer(app->window->GetWindowWidth(), app->window->GetWindowHeight());
				//camera->SetOwner(gamObj);
				camera->SetIsMainCamera(tmp.JsonValToBool("isMainCamera"));
				camera->SetShowFrustrum(tmp.JsonValToBool("showFrustrum"));

				break;
			}

			//gamObj->AddComponent();

		}
		else break;
	}

}

float4x4 Scene::strMatrixToF4x4(const char* convert)
{
	std::string text = convert;
	std::string delimiter = ",";
	std::vector<float> floatArray{};

	size_t pos = 0;
	while ((pos = text.find(delimiter)) != std::string::npos) {
		floatArray.push_back(stof(text.substr(0, pos)));
		text.erase(0, pos + delimiter.length());
	}

	float4x4 matrix;
	int count = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{

			matrix.At(i, j) = floatArray.at(count);
			++count;
		}

	return matrix;
}