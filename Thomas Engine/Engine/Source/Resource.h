#pragma once
#include<string>

enum class ResourceType {
	TEXTURE,
	MODEL,
	MESH,
	SCENE,
	SCRIPT,
	SHADER,
	MATERIAL,
	UNKNOWN
};

class Resource
{
public:
	

public:
	Resource(int uid, ResourceType type);
	virtual ~Resource();

	inline ResourceType GetType() const { return type; }
	inline int GetUID() const { return uid; }
	inline const char* GetAssetPath() const { return assetsFile.c_str(); }
	inline const char* GetLibraryPath() const { return libraryFile.c_str(); }

	void SetAssetsPath(const char*);
	void SetLibraryPath(const char*);

	//inline bool IsLoadedToMemory() const { return (referenceCount >= 0) ? true : false; }

	inline unsigned int GetReferenceCount() const { return referenceCount; }
	inline void IncreaseReferenceCount() { referenceCount++; }
	inline void DecreaseReferenceCount() { referenceCount--; }

	//Can't be pure virtual "=0" until there is a resoruce model class
	virtual bool LoadToMemory() { return false; }
	virtual bool UnloadFromMemory() { return false; }

public:

	int uid;
	std::string assetsFile;
	std::string libraryFile;
	ResourceType type = ResourceType::UNKNOWN;
	unsigned int referenceCount;
};