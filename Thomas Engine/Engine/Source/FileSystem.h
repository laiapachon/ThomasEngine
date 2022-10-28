#ifndef __FileSystem_H__
#define __FileSystem_H__

#include <vector>
#include <string>

#include "JsonParser.h"
#include "PhysFS/include/physfs.h"


struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

struct aiFileIO;

class FileSystem 
{
public:

	FileSystem(const char* assetsPath);
	FileSystem();

	// Destructor
	~FileSystem();

	// Called before render is available
	bool Init(JsonParser& config);

	// Called before quitting
	bool CleanUp();

	// Utility functions
	bool AddPath(const char* path_or_zip);

	// Open for Read/Write
	unsigned int Load(const char* file, char** buffer) const;
	uint Save(const char* file, const void* buffer, unsigned int size, bool append = false);

	// IO interfaces for other libs to handle files via PHYSfs
	aiFileIO* GetAssimpIO();


	inline const char* GetBasePath() const { return PHYSFS_getBaseDir(); }
	inline const char* GetWritePath() const { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths() const;

private:

	void CreateAssimpIO();

private:

	aiFileIO* assimpIO = nullptr;
	const char* name;

};

#endif // __FileSystem_H__
