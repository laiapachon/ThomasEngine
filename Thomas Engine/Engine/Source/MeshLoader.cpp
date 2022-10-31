#include "Application.h"
#include "MeshLoader.h"

// Importers
#include "FileSystem.h"
#include "TextureLoader.h"

// Resources
#include "ResourceMesh.h"
#include "ResourceTexture.h"

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
	// Detach log stream
	aiDetachAllLogStreams();
}

Mesh* MeshLoader::LoadMesh(aiMesh* importedMesh, uint oldUID)
{
	uint UID = oldUID;
	if (UID == 0)
		UID = app->GetNewUID();

	LOG(LogType::L_NORMAL, "%s", importedMesh->mName.C_Str());
	std::string file = MESHES_FOLDER;
	file += std::to_string(UID);
	file += ".mmh";

	Mesh* mesh = new Mesh(UID);

	// copy vertices
	mesh->numVertex = importedMesh->mNumVertices;
	//mesh->vertices.resize(mesh->numVertex * sizeof(float) * 3);

	mesh->numTexCoords = importedMesh->mNumVertices;
	//mesh->texCoords.resize(mesh->numTexCoords * sizeof(float) * 2);

	mesh->numNormals = importedMesh->mNumVertices;
	//mesh->normals.resize(mesh->numNormals * sizeof(float) * 3);

	for (size_t i = 0; i < mesh->numVertex; i++)
	{
		mesh->vertices.push_back(importedMesh->mVertices[i].x);
		mesh->vertices.push_back(importedMesh->mVertices[i].y);
		mesh->vertices.push_back(importedMesh->mVertices[i].z);

		if (importedMesh->HasNormals())
		{
			mesh->normals.push_back(importedMesh->mNormals[i].x);
			mesh->normals.push_back(importedMesh->mNormals[i].y);
			mesh->normals.push_back(importedMesh->mNormals[i].z);
		}
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
		mesh->indices.resize(mesh->numIndices);

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

	mesh->LoadToMemory();

	LOG(LogType::L_NORMAL, "New mesh with %d vertices", mesh->numVertex);
	LOG(LogType::L_NORMAL, "New mesh with %d normals", mesh->numNormals);
	LOG(LogType::L_NORMAL, "New mesh with %d texture coords", mesh->numTexCoords);

	return mesh;
}