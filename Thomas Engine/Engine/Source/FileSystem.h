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
	ImportType GetImportType(const char* path);

	void FSInit();
	void FSDeInit();
	void LibraryMaker();

	bool AddPath(const char* path_or_zip);
	bool FileExist(const char* file);
	bool CreateDir(const char* dir);
	bool IsDirFound(const char* file);

	std::string NormalizePath(const char* fullPath);
	std::string UnNormalizePath(const char* fullPath);

	std::string ExtractLocalDiskBackward(const char* fullPath);
	std::string ExtractLocalDiskForward(const char* fullPath);

	unsigned int FileLoad(const char* file, char** buffer);
	unsigned int FileSave(const char* file, const void* buffer, unsigned int size, bool append = false);

	bool FileRemove(const char* file);
	void GetFileName(const char* file, std::string& fileName, bool extension);

	inline const char* GetBasePath() { return PHYSFS_getBaseDir(); }
	inline const char* GetWritePath() { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths();

	void OnGui();
};