#include "Application.h"
#include "ResourceManager.h"

//Importers
#include "FileSystem.h"
#include "TextureLoader.h"
#include "MeshLoader.h"
#include "ModelImporter.h"

//Resources
#include "ResourceTexture.h"
#include "ResourceMesh.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "ResourceManager";
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Init()
{
	return true;
}

bool ResourceManager::Start()
{
	return true;
}

bool ResourceManager::CleanUp()
{
	return true;
}
// Generate unique UID
int ResourceManager::GenerateNewUID()
{
	return app->GetNewUID();
}
int ResourceManager::GetMetaUID(const char* metaFile) const
{
	JSON_Value* metaJSON = json_parse_file(metaFile);
	JsonConfig rObj(json_value_get_object(metaJSON));

	uint mUID = rObj.ReadInt("UID");

	//Free memory
	json_value_free(metaJSON);

	return mUID;
}
int ResourceManager::ImportFile(const char* assetsFile, Resource::Type type)
{
	//To check if a resource is loaded we need to create .meta files first

	if (type == Resource::Type::UNKNOWN)
		return 0;

	//Generate meta
	std::string meta = ResourceManager::GetMetaPath(assetsFile);
	uint resUID = GetMetaUID(meta.c_str());

	Resource* resource = GetResourceFromUID(resUID);

	bool isCreated = false;
	if (resource == nullptr) {
		resource = CreateNewResource(assetsFile, resUID, type);
		isCreated = true;
	}

	if (resource == nullptr)
		return 0;

	int ret = 0;

	char* fileBuffer = nullptr;
	unsigned int size = FileSystem::LoadToBuffer(assetsFile, &fileBuffer);

	static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 7, "Update all switches with new type");
	switch (resource->GetType())
	{
		case Resource::Type::TEXTURE: TextureLoader::Import(fileBuffer, size, resource); break;
		case Resource::Type::MODEL: ModelImporter::Import(fileBuffer, size, resource); break;
	}

	//Save the resource to custom format
	ret = resource->GetUID();

	RELEASE_ARRAY(fileBuffer);

	if (resource->GetReferenceCount() <= 1 && isCreated == true)
		UnloadResource(ret);

	return ret;
}
void ResourceManager::UnloadResource(int uid)
{
	Resource* res = nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;

	res = it->second;
	res->DecreaseReferenceCount();

	if (res->GetReferenceCount() <= 0)
		ReleaseResource(res->GetUID());
}
// Get file type according to its extension
Resource::Type ResourceManager::GetTypeFromLibraryExtension(const char* libraryFile) const
{
	std::string ext(libraryFile);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "dds")
		return Resource::Type::TEXTURE;
	if (ext == "model")
		return Resource::Type::MODEL;
	if (ext == "mmh")
		return Resource::Type::MESH;
	if (ext == "des")
		return Resource::Type::SCENE;
	if (ext == "shdr")
		return Resource::Type::SHADER;
	if (ext == "mat")
		return Resource::Type::MATERIAL;


	return Resource::Type::UNKNOWN;
}
void ResourceManager::UpdateMeshesDisplay()
{
	/*meshesLibraryRoot.childDirs.clear();
	App->moduleFileSystem->GetAllFilesRecursive(meshesLibraryRoot);
	LOG(LogType::L_WARNING, "Mesh display updated");*/
}
void ResourceManager::ReleaseResource(int uid)
{
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;

	Resource* res = (*it).second;
	(*it).second->UnloadFromMemory();
	resources.erase((*it).second->GetUID());
	delete res;
}
Resource* ResourceManager::CreateNewResource(const char* assetsFile, uint uid, Resource::Type type)
{
	Resource* ret = nullptr;

	static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 7, "Update all switches with new type");
	switch (type)
	{
	case Resource::Type::TEXTURE: ret = (Resource*) new Texture(uid); break;
	case Resource::Type::MESH: ret = (Resource*) new Mesh(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->SetAssetsPath(assetsFile);
		ret->SetLibraryPath(GenLibraryPath(ret->GetUID(), type).c_str());
		ret->IncreaseReferenceCount();
	}

	return ret;
}
// Create new Resource
Resource* ResourceManager::RequestResource(int uid, const char* libraryPath)
{
	//Find if the resource is already loaded
	if (uid <= -1)
		return nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		it->second->IncreaseReferenceCount();
		return it->second;
	}

	//Find the library file (if exists) and load the custom file format
	if (libraryPath != nullptr)
	{
		Resource* ret = nullptr;

		static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 7, "Update all switches with new type");

		//Save check
		if (FileSystem::Exists(libraryPath))
		{
			//uid = 0; //This should be the uid from library
			switch (GetTypeFromLibraryExtension(libraryPath))
			{
				case Resource::Type::TEXTURE: ret = (Resource*) new Texture(uid); break;
				case Resource::Type::MESH: ret = (Resource*) new Mesh(uid); break;
			}

			if (ret != nullptr)
			{
				resources[uid] = ret;
				ret->SetAssetsPath("");
				ret->SetLibraryPath(libraryPath);
				ret->IncreaseReferenceCount();

				ret->LoadToMemory();
			}
			LOG(LogType::L_NORMAL, "Requested resource loaded as new");
		}
		else
			LOG(LogType::L_ERROR, "Requested resource does not exist");

		return ret;
	}

	return nullptr;
}

Resource* ResourceManager::GetResourceFromUID(int uid)
{
	//Find if the resource is already loaded
	if (uid <= -1)
		return nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		return it->second;
	}

	return nullptr;
}

std::string ResourceManager::GetMetaPath(const char* assetsFile)
{
	std::string metaFile(assetsFile);
	metaFile += ".meta";
	return metaFile;
}

std::string ResourceManager::GenLibraryPath(uint _uid, Resource::Type _type)
{
	std::string ret = "";
	std::string nameNoExt = std::to_string(_uid);

	switch (_type)
	{
	case Resource::Type::TEXTURE: ret = TEXTURES_FOLDER; ret += nameNoExt; ret += ".dds"; break;
	case Resource::Type::MESH: ret = MESH_FOLDER; ret += nameNoExt; ret += ".mmh"; break;
	}

	return ret;
}
