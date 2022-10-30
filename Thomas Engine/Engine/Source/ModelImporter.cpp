#include "Application.h"
#include "ModelImporter.h"
#include "Globals.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "FileSystem.h"
#include "MeshLoader.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


void ModelImporter::Import(char* buffer, int bSize, Resource* res)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, bSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<Mesh*> meshesOnModelUIDs;

		//Load mesh uid from meta file
		std::vector<uint> uids;
		if (FileSystem::Exists(app->resources->GetMetaPath(res->GetAssetPath()).c_str()))
			GetMeshesFromMeta(res->GetAssetPath(), uids);

		if (scene->HasMeshes())
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				if (uids.size() != 0)
					meshesOnModelUIDs.push_back(MeshLoader::LoadMesh(scene->mMeshes[i], uids[i]));
				else
					meshesOnModelUIDs.push_back(MeshLoader::LoadMesh(scene->mMeshes[i]));
			}
		}

		SaveMeshesToMeta(res->GetAssetPath(), meshesOnModelUIDs);
		app->resources->UpdateMeshesDisplay();

		meshesOnModelUIDs.clear();
		uids.clear();

		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene"/*, scene->name*/);
}

void ModelImporter::SaveMeshesToMeta(const char* assetFile, std::vector<Mesh*>& meshes)
{
	JSON_Value* metaFile = json_parse_file(app->resources->GetMetaPath(assetFile).c_str());

	if (metaFile == NULL)
		return;

	JSON_Object* sceneObj = json_value_get_object(metaFile);

	JSON_Value* arrayVal = json_value_init_array();
	JSON_Array* meshArray = json_value_get_array(arrayVal);

	for (size_t i = 0; i < meshes.size(); i++)
	{
		json_array_append_number(meshArray, meshes[i]->GetUID());
	}
	json_object_set_value(sceneObj, "Meshes Inside", arrayVal);

	json_serialize_to_file_pretty(metaFile, app->resources->GetMetaPath(assetFile).c_str());

	//Free memory
	json_value_free(metaFile);
}

void ModelImporter::GetMeshesFromMeta(const char* assetFile, std::vector<uint>& uids)
{
	JSON_Value* metaFile = json_parse_file(app->resources->GetMetaPath(assetFile).c_str());

	if (metaFile == NULL)
		return;

	JSON_Object* sceneObj = json_value_get_object(metaFile);

	JSON_Array* meshArray = json_object_get_array(sceneObj, "Meshes Inside");

	for (size_t i = 0; i < json_array_get_count(meshArray); i++)
	{
		uids.push_back(json_array_get_number(meshArray, i));
	}

	//Free memory
	json_value_free(metaFile);
}
