#pragma once
#include "PhysFS/include/physfs.h"

#include <vector>
#include <string>

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};

namespace StringLogic {

	std::string FileNameFromPath(const char* path);
	std::string GlobalToLocalPath(const char* globalPath);
}

namespace FileSystem 
{
	ImportType GetTypeFromPath(const char* path);

	void FSInit();
	void FSDeInit();
	void CreateLibraryDirectories();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file);
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file);

	std::string NormalizePath(const char* fullPath);
	std::string UnNormalizePath(const char* fullPath);

	std::string ExtractLocalDiskBackward(const char* fullPath);
	std::string ExtractLocalDiskForward(const char* fullPath);

	// Open for Read/Write
	unsigned int LoadToBuffer(const char* file, char** buffer);

	bool Remove(const char* file);
	void GetFileName(const char* file, std::string& fileName, bool extension);

	inline const char* GetBasePath() { return PHYSFS_getBaseDir(); }
	inline const char* GetWritePath() { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths();

	void OnGui();
};