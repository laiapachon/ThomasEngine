#include "MeshLoader.h"
#include "Application.h"
#include "Editor.h"
#include "ResourceMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

void MeshLoader::LogCallback(const char* message, char* user)
{
	app->editor->LogToConsole(message, LogType::L_NORMAL);
}
void MeshLoader::EnableDebugMode()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);
}
void MeshLoader::DisableDebugMode()
{
	// detach log stream
	aiDetachAllLogStreams();
}

Mesh* MeshLoader::LoadMesh(aiMesh* importedMesh, uint oldUID)
{
	uint UID = oldUID;
	if (UID == 0)
		UID = app->GetNewUID();

	LOG(LogType::L_NORMAL, "%s", importedMesh->mName.C_Str());
	std::string file = MESH_FOLDER;
	file += std::to_string(UID);
	file += ".mmh";

	Mesh* mesh = dynamic_cast<Mesh*>(app->resources->CreateNewResource("", UID, Resource::Type::MESH));

	// copy vertices
	mesh->vertices.resize(mesh->numVertex * sizeof(float) * 3);
	mesh->texCoords.resize(mesh->numVertex * sizeof(float) * 2);

	for (size_t i = 0; i < mesh->numVertex; i++)
	{
		mesh->vertices.push_back(importedMesh->mVertices[i].x);
		mesh->vertices.push_back(importedMesh->mVertices[i].y);
		mesh->vertices.push_back(importedMesh->mVertices[i].z);

		if (importedMesh->HasTextureCoords(0))
		{
			mesh->texCoords.push_back(importedMesh->mTextureCoords[0][i].x);
			mesh->texCoords.push_back(importedMesh->mTextureCoords[0][i].y);
		}
	}

	if (importedMesh->HasVertexColors(0))
	{
		LOG(LogType::L_ERROR, "ADD VERTEX COLORS");
	}


	// Generate indices
	if (importedMesh->HasFaces())
	{
		mesh->numIndices = importedMesh->mNumFaces * 3;
		mesh->indices.resize(mesh->numVertex);
		
		for (uint j = 0; j < importedMesh->mNumFaces; ++j)
		{
			if (importedMesh->mFaces[j].mNumIndices != 3)
			{
				LOG(LogType::L_WARNING, "WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&mesh->indices[j * 3], importedMesh->mFaces[j].mIndices, 3 * sizeof(uint));
			}
		}
	}

	return mesh;
}
