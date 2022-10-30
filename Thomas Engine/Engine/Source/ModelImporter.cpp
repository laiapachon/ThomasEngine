#include "Application.h"
#include "ModelImporter.h"
#include "Globals.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "FileSystem.h"
#include "MeshLoader.h"
#include "TextureLoader.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


void ModelImporter::Import(const char* fullPath, char* buffer, int bSize, Resource* res)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, bSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	std::string fileName = StringLogic::FileNameFromPath(fullPath);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<Mesh*> sceneMeshes;
		std::vector<Texture*> testTextures;

		//This should not be here
		if (scene->HasMaterials())
		{
			//Needs to be moved to another place
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

					std::string localPath = StringLogic::GlobalToLocalPath(fullPath);
					localPath = localPath.substr(0, localPath.find_last_of('/') + 1);
					localPath += FileSystem::NormalizePath(path.C_Str());

					char* buffer = nullptr;
					int size = FileSystem::LoadToBuffer(localPath.c_str(), &buffer);

					if (buffer != nullptr)
					{
						int w = 0;
						int h = 0;

						GLuint id = TextureLoader::LoadToMemory(buffer, size, &w, &h);
						Texture* Test = new Texture(id, w, h);
						testTextures.push_back(Test);

						RELEASE_ARRAY(buffer)
					}

					path.Clear();
				}
			}
		}

		//Load all meshes into mesh vector
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			sceneMeshes.push_back(MeshLoader::LoadMesh(scene->mMeshes[i]));
		}

		// META
		//LOG(LogType::L_NORMAL, "-- Loading FBX as GameObject --");
		//NodeToGameObject(scene->mMeshes, testTextures, sceneMeshes, scene->mRootNode, gmRoot, fileName.c_str());

		//Only for memory cleanup, needs an update ASAP
		for (unsigned int i = 0; i < sceneMeshes.size(); i++)
		{
			app->renderer3D->globalMeshes.push_back(sceneMeshes[i]);
		}
		for (unsigned int i = 0; i < testTextures.size(); i++)
		{
			app->renderer3D->globalTextures.push_back(testTextures[i]);
		}

		sceneMeshes.clear();
		testTextures.clear();

		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene % s", fullPath);
}
