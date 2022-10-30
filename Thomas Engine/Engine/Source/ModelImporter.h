#pragma once
#include<vector>

class Resource;
class Mesh;
typedef unsigned int uint;

namespace ModelImporter
{
	void Import(const char* fullPath, char* buffer, int bSize, Resource* res);
}