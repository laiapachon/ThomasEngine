#include "Resource.h"
#include"Globals.h"

Resource::Resource(ResourceType type) : assetFile(""), libFile(""), type(type), refCount(0)
{

}

Resource::~Resource()
{
	assetFile.clear();
	libFile.clear();
	type = ResourceType::UNKNOWN;

	if (refCount != 0)
		LOG(LogType::L_NORMAL, "Deleting Resource");

	refCount = 0;
}

void Resource::SetAssetsPath(const char* _aPath)
{
	assetFile = _aPath;
}

void Resource::SetLibraryPath(const char* _lPath)
{
	libFile = _lPath;
}

const char* Resource::GetNewFormat()
{
	switch (type)
	{
	//case ResourceType::TEXTURE: return "thtex";
	case ResourceType::MESH: return "thmesh";
	default: return "unknown";
	}
}