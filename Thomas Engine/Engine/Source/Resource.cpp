#include "Resource.h"
#include"Globals.h"

Resource::Resource(int uid, ResourceType type) : uid(uid), assetsFile(""), libraryFile(""), type(type), referenceCount(0)
{

}

Resource::~Resource()
{
	uid = 0;
	assetsFile.clear();
	libraryFile.clear();
	type = ResourceType::UNKNOWN;

	if (referenceCount != 0)
		LOG(LogType::L_NORMAL, "DELETING RESOURCE WITH MULTIPLE REFERENCE COUNTS");

	referenceCount = 0;
}

void Resource::SetAssetsPath(const char* _aPath)
{
	assetsFile = _aPath;
}

void Resource::SetLibraryPath(const char* _lPath)
{
	libraryFile = _lPath;
}
