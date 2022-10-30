#ifndef __FileSystem_H__
#define __FileSystem_H__

#include <vector>
#include <string>

#include "JsonParser.h"
#include "PhysFS/include/physfs.h"


struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

struct aiFileIO;

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};

namespace StringLogic {
	std::string GlobalToLocalPath(const char* _globalPath);
}

namespace FileSystem 
{

	ImportType GetTypeFromPath(const char* path);

	void FSInit();
	void FSDeInit();
	void GetFileName(const char* file, std::string& fileName, bool extension);
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) /*const*/;

	// Utility functions
	bool Exists(const char* file) /*const*/;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) /*const*/;
	bool AddPath(const char* path_or_zip);

	std::string NormalizePath(const char* path) /*const*/;
	std::string UnNormalizePath(const char* full_path);


	// Open for Read/Write
	unsigned int LoadToBuffer(const char* file, char** buffer) /*const*/;
	unsigned int Load(const char* path, const char* file, char** buffer) /*const*/;

	unsigned int Save(const char* file, char* buffer, unsigned int size, bool append);
	unsigned int Copy(const char* file, const char* dir, std::string& outputFile);

	inline const char* GetBasePath() { return PHYSFS_getBaseDir(); }
	inline const char* GetWritePath() { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths();

	// IO interfaces for other libs to handle files via PHYSfs
	//aiFileIO* GetAssimpIO();

	//void CreateAssimpIO();

	//aiFileIO* assimpIO = nullptr;

};

#endif // __FileSystem_H__
