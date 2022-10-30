#pragma once
#include<vector>

class Resource;
class Mesh;
typedef unsigned int uint;

namespace ModelImporter
{
	void Import(char* buffer, int bSize, Resource* res);

	void SaveMeshesToMeta(const char* assetFile, std::vector<Mesh*>& meshes);
	void GetMeshesFromMeta(const char* assetFile, std::vector<uint>& uids);
}