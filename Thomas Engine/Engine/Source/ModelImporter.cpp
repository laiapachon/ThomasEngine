#include "Application.h"
#include "ModelImporter.h"
#include "Globals.h"

// Components
#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"

// Importers
#include "FileSystem.h"
#include "MeshLoader.h"
#include "TextureLoader.h"

// Resources
#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include"MathGeoLib/include/Math/Quat.h"

void ModelImporter::Import(const char* fullPath,  char* buffer, int bufferSize, GameObject* root)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, bufferSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	std::string fileName = StringLogic::FileNameFromPath(fullPath);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<Mesh*> modelMeshes;
		std::vector<Texture*> modelTextures;

		LOG(LogType::L_NORMAL, "Loading meshes from %c", fileName.c_str());
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			modelMeshes.push_back(MeshLoader::LoadMesh(scene->mMeshes[i]));
		}
		LOG(LogType::L_NORMAL, "Loading materials from %s", fileName.c_str());
		LoadMaterials(scene, fullPath, modelTextures);

		LOG(LogType::L_NORMAL, "Loading model as GameObject from %s", fileName.c_str());
		NodeToGameObject(scene->mMeshes, modelTextures, modelMeshes, scene->mRootNode, root, fileName.c_str());

		modelMeshes.clear();
		modelTextures.clear();
		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading model % s", fullPath);
}

void ModelImporter::LoadMaterials(const aiScene* scene, const char* fullPath, std::vector<Texture*>& testTextures)
{
	if (scene->HasMaterials())
	{
		std::string generalPath(fullPath);
		generalPath = generalPath.substr(0, generalPath.find_last_of("/\\") + 1);
		for (size_t k = 0; k < scene->mNumMaterials; k++)
		{
			aiMaterial* material = scene->mMaterials[k];			
			uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
			
			if (numTextures > 0)
			{
				aiString path;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

				std::string localPath(fullPath);
				localPath = localPath.substr(0, localPath.find_last_of('/') + 1);

				std::string fileNamePath = FileSystem::NormalizePath(path.C_Str());
				fileNamePath = fileNamePath.substr(fileNamePath.find_last_of("/\\") + 1);

				
				localPath += fileNamePath;

				
				char* buffer = nullptr;
				FileSystem::FileLoad(localPath.c_str(), &buffer);

				if (buffer != nullptr)
				{
					Texture* tex = new Texture(localPath.c_str());
					tex->LoadToMemory();

					testTextures.push_back(tex);

					RELEASE_ARRAY(buffer)
				}

				path.Clear();
			}
		}
	}
}

void ModelImporter::NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& sceneMeshes, aiNode* node, GameObject* objParent, const char* ownerName)
{
	FillGameObject(node, sceneMeshes, objParent, meshArray, sceneTextures);

	if (node->mNumChildren > 0)
	{
		GameObject* rootGO = objParent;
		
		if (node->mNumChildren == 1 && node->mParent == nullptr && node->mChildren[0]->mNumChildren == 0)
		{
			node->mChildren[0]->mName = ownerName;
		}
		else
		{			
			rootGO = new GameObject(ownerName);
			
			rootGO->SetParent(objParent);
			
			PopulateTransform(rootGO, node);
			
			objParent->AddChildren(rootGO);
		}
		
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(meshArray, sceneTextures, sceneMeshes, node->mChildren[i], rootGO, node->mChildren[i]->mName.C_Str());
		}
	}
}

void ModelImporter::FillGameObject(aiNode* node, std::vector<Mesh*>& sceneMeshes, GameObject* objParent, aiMesh** meshArray, std::vector<Texture*>& sceneTextures)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		
		GameObject* gmEmpty = new GameObject(node->mName.C_Str());
		gmEmpty->SetParent(objParent);

		
		MeshRenderer* gmMeshRenderer = static_cast<MeshRenderer*>(gmEmpty->AddComponent(ComponentType::MESH_RENDERER));
		gmMeshRenderer->SetMesh(sceneMeshes[node->mMeshes[i]]);
		gmMeshRenderer->SetBoundingBoxes(sceneMeshes[node->mMeshes[i]]);

		
		aiMesh* importedMesh = meshArray[node->mMeshes[i]];
		if (importedMesh->mMaterialIndex < sceneTextures.size())
		{
			Material* material = static_cast<Material*>(gmEmpty->AddComponent(ComponentType::MATERIAL));
			material->texture = sceneTextures[importedMesh->mMaterialIndex];
		}
		
		PopulateTransform(gmEmpty, node);
		objParent->AddChildren(gmEmpty);
	}
}

void ModelImporter::PopulateTransform(GameObject* child, aiNode* node)
{
	aiVector3D translation, scaling;
	aiQuaternion rotation;
	
	node->mTransformation.Decompose(scaling, rotation, translation);
	
	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	
	child->transform->SetTransformMatrix(pos, rot, scale, child->GetParent()->transform);
}
