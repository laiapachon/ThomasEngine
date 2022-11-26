#pragma once

class Mesh;
class aiMesh;
typedef unsigned int uint;

namespace MeshLoader
{
	void EnableDebugMode();
	void DisableDebugMode();
	void LogCallback(const char* message, char* user);

	Mesh* LoadMesh(aiMesh* importedMesh);
}