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
	Resource(ResourceType type);
	virtual ~Resource();

	inline ResourceType GetType() const { return type; }
	inline const char* GetAssetPath() const { return assetFile.c_str(); }
	inline const char* GetLibraryPath() const { return libFile.c_str(); }

	void SetAssetsPath(const char*);
	void SetLibraryPath(const char*);	

	inline unsigned int GetRefCount() const { return refCount; }
	inline void IncreaseRefCount() { refCount++; }
	inline void DecreaseRefCount() { refCount--; }

	virtual bool LoadToMemory() { return false; }
	virtual bool UnloadFromMemory() { return false; }
	const char* GetNewFormat();

private:
	std::string assetFile;
	std::string libFile;
	ResourceType type = ResourceType::UNKNOWN;
	unsigned int refCount;
};