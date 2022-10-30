#include "Globals.h"
#include "Application.h"
#include "FileSystem.h"


//#include "Assimp/include/cfileio.h"
//#include "Assimp/include/types.h"


//#include "Leaks.h"


/*Convert global path to local path*/
std::string StringLogic::GlobalToLocalPath(const char* _globalPath)
{
	std::string localPath = FileSystem::NormalizePath(_globalPath);

	size_t pos = 0;
	pos = localPath.find(ASSETS_FOLDER);
	if (pos != std::string::npos)
	{
		localPath = localPath.substr(pos);
		if (localPath.c_str() != "")
			return localPath;
	}

	localPath.clear();
	return localPath;
}

/*Get the type of importer using any path with a file extension*/
ImportType FileSystem::GetTypeFromPath(const char* path)
{
	std::string ext(path);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx" || ext == "dae")
		return ImportType::MESH;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "dds")
		return ImportType::TEXTURE;

	return ImportType::NOTYPE;
}

/*Init PhysFS*/
void FileSystem::FSInit()
{
	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	//Setting the working directory as the writing directory
	if (PHYSFS_setWriteDir(".") == 0)
		LOG(LogType::L_NORMAL, "File System error while creating write dir: %s\n", PHYSFS_getLastError());

	FileSystem::AddPath("."); //Adding ProjectFolder (working directory)
	FileSystem::AddPath("Assets");

	// Dump list of paths
	LOG(LogType::L_NORMAL, "FileSystem Operations base is [%s] plus:", GetBasePath());
	LOG(LogType::L_NORMAL, GetReadPaths());

	// Make sure standard paths exist
	const char* dirs[] = {
		SETTINGS_FOLDER, ASSETS_FOLDER
	};

	for (uint i = 0; i < sizeof(dirs) / sizeof(const char*); ++i)
	{
		if (PHYSFS_exists(dirs[i]) == 0)
			PHYSFS_mkdir(dirs[i]);
	}

	// Generate IO interfaces
	//CreateAssimpIO();
}

void FileSystem::FSDeInit()
{
	PHYSFS_deinit();
}

// Add a new zip file or folder
bool FileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG(LogType::L_ERROR, "File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}
const char* FileSystem::GetReadPaths()
{
	static char paths[512];

	paths[0] = '\0';

	char** path;
	for (path = PHYSFS_getSearchPath(); *path != nullptr; path++)
	{
		strcat_s(paths, 512, *path);
		strcat_s(paths, 512, "\n");
	}

	return paths;
}

// Check if a file exists
bool FileSystem::Exists(const char* file) /*const*/
{
	return PHYSFS_exists(file) != 0;
}

bool FileSystem::CreateDir(const char* dir)
{
	if (IsDirectory(dir) == false)
	{
		PHYSFS_mkdir(dir);
		return true;
	}
	return true;
}

// Check if a file is a directory
bool FileSystem::IsDirectory(const char* file) /*const*/
{
	return PHYSFS_isDirectory(file) != 0;
}

//*PHYSFS_getSearchPath() not working and causing a crash
//void FileSystem::GetRealDir(const char* path, std::string& output)
//{
//	output = PHYSFS_getBaseDir();
//
//	std::string baseDir = PHYSFS_getBaseDir();
//	std::string searchPath = *PHYSFS_getSearchPath();
//	std::string realDir = PHYSFS_getRealDir(path);
//
//	output.append(*PHYSFS_getSearchPath()).append("/");
//	output.append(PHYSFS_getRealDir(path)).append("/").append(path);
//}
//std::string FileSystem::GetPathRelativeToAssets(const char* originalPath) /*const*/
//{
//	std::string ret;
//	GetRealDir(originalPath, ret);
//
//	return ret;
//}

std::string FileSystem::NormalizePath(const char* full_path) /*const*/
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}
std::string FileSystem::UnNormalizePath(const char* full_path) /*const*/
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '/')
			newPath[i] = '\\';
	}
	return newPath;
}

void FileSystem::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) /*const*/
{
	if (full_path != nullptr)
	{
		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				*path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			else
				*file = full.substr(0, pos_dot);
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				*extension = full.substr(pos_dot + 1);
			else
				extension->clear();
		}
	}
}

unsigned int FileSystem::Load(const char* path, const char* file, char** buffer) /*const*/
{
	std::string full_path(path);
	full_path += file;
	return LoadToBuffer(full_path.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint FileSystem::LoadToBuffer(const char* file, char** buffer) /*const*/
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);
		//LOG(LogType::L_ERROR, "[%s]", PHYSFS_getLastError())

		if (size > 0)
		{
			*buffer = new char[size + 1];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);
			if (readed != size)
			{
				LOG(LogType::L_ERROR, "File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
			{
				ret = readed;
				//Adding end of file at the end of the buffer. Loading a shader file does not add this for some reason
				(*buffer)[size] = '\0';
			}
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG(LogType::L_ERROR, "File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

/*Duplicate a file to a local PhysFS valid path*/
unsigned int FileSystem::Copy(const char* file, const char* dir, std::string& outputFile)
{
	uint size = 0;

	if (file == nullptr || dir == nullptr)
	{
		/*assert(file != nullptr && dir != nullptr);*/
		return size;
	}

	std::FILE* filehandle;
	fopen_s(&filehandle, file, "rb");

	if (filehandle != nullptr)
	{
		fseek(filehandle, 0, SEEK_END);
		size = ftell(filehandle);
		rewind(filehandle);

		char* buffer = new char[size];
		size = fread(buffer, 1, size, filehandle);
		if (size > 0)
		{
			GetFileName(file, outputFile, true);
			outputFile.insert(0, "/");
			outputFile.insert(0, dir);

			size = Save(outputFile.data(), buffer, size, false);
			if (size > 0)
			{
				LOG(LogType::L_NORMAL, "FILE SYSTEM: Successfully copied file '%s' in dir '%s'", file, dir);
			}
			else
				LOG(LogType::L_ERROR, "FILE SYSTEM: Could not copy file '%s' in dir '%s'", file, dir);
		}
		else
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not read from file '%s'", file);

		RELEASE_ARRAY(buffer);
		fclose(filehandle);
	}
	else
		LOG(LogType::L_ERROR, "FILE SYSTEM: Could not open file '%s' to read", file);

	return size;
}


//std::string FileSystem::FileToText(const char* path)
//{
//	// Read from the text file
//	std::ifstream readFile(path);
//
//	std::stringstream buffer;
//	buffer << readFile.rdbuf();
//
//	// Close the file
//	readFile.close();
//
//	return buffer.str();
//}

void FileSystem::GetFileName(const char* file, std::string& fileName, bool extension)
{
	fileName = file;

	uint found = fileName.find_last_of("\\");
	if (found != std::string::npos)
		fileName = fileName.substr(found + 1, fileName.size());

	found = fileName.find_last_of("//");
	if (found != std::string::npos)
		fileName = fileName.substr(found + 1, fileName.size());

	if (!extension)
	{
		found = fileName.find_last_of(".");
		if (found != std::string::npos)
			fileName = fileName.substr(0, found);
	}
}

unsigned int FileSystem::Save(const char* file, char* buffer, unsigned int size, bool append)
{
	uint objCount = 0;

	std::string fileName;
	GetFileName(file, fileName, true);


	bool exists = Exists(file);

	PHYSFS_file* filehandle = nullptr;
	if (append)
		filehandle = PHYSFS_openAppend(file);
	else
		filehandle = PHYSFS_openWrite(file);

	if (filehandle != nullptr)
	{
		objCount = PHYSFS_writeBytes(filehandle, (const void*)buffer, size);

		if (objCount == size)
		{
			if (exists)
			{
				if (append)
				{
					LOG(LogType::L_NORMAL, "FILE SYSTEM: Append %u bytes to file '%s'", objCount, fileName.data());
				}
				else
					LOG(LogType::L_NORMAL, "FILE SYSTEM: File '%s' overwritten with %u bytes", fileName.data(), objCount);
			}
			else
				LOG(LogType::L_NORMAL, "FILE SYSTEM: New file '%s' created with %u bytes", fileName.data(), objCount);
		}
		else
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not write to file '%s'. ERROR: %s", fileName.data(), PHYSFS_getLastError());

		if (PHYSFS_close(filehandle) == 0)
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not close file '%s'. ERROR: %s", fileName.data(), PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "FILE SYSTEM: Could not open file '%s' to write. ERROR: %s", fileName.data(), PHYSFS_getLastError());

	return objCount;
}


// -----------------------------------------------------
// ASSIMP IO
// -----------------------------------------------------

//size_t AssimpWrite(aiFile* file, const char* data, size_t size, size_t chunks)
//{
//	PHYSFS_sint64 ret = PHYSFS_write((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
//	if(ret == -1)
//		LOG(LogType::L_ERROR, "File System error while WRITE via assimp: %s", PHYSFS_getLastError());
//
//	return (size_t) ret;
//}
//
//size_t AssimpRead(aiFile* file, char* data, size_t size, size_t chunks)
//{
//	PHYSFS_sint64 ret = PHYSFS_read((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
//	if(ret == -1)
//		LOG(LogType::L_ERROR, "File System error while READ via assimp: %s", PHYSFS_getLastError());
//
//	return (size_t) ret;
//}
//
//size_t AssimpTell(aiFile* file)
//{
//	PHYSFS_sint64 ret = PHYSFS_tell((PHYSFS_File*)file->UserData);
//	if(ret == -1)
//		LOG(LogType::L_ERROR, "File System error while TELL via assimp: %s", PHYSFS_getLastError());
//
//	return (size_t) ret;
//}
//
//size_t AssimpSize(aiFile* file)
//{
//	PHYSFS_sint64 ret = PHYSFS_fileLength((PHYSFS_File*)file->UserData);
//	if(ret == -1)
//		LOG(LogType::L_ERROR, "File System error while SIZE via assimp: %s", PHYSFS_getLastError());
//
//	return (size_t) ret;
//}
//
//void AssimpFlush(aiFile* file)
//{
//	if(PHYSFS_flush((PHYSFS_File*)file->UserData) == 0)
//		LOG(LogType::L_ERROR, "File System error while FLUSH via assimp: %s", PHYSFS_getLastError());
//}
//
//aiReturn AssimpSeek(aiFile* file, size_t pos, aiOrigin from)
//{
//	int res = 0;
//
//	switch (from)
//	{
//	case aiOrigin_SET:
//		res = PHYSFS_seek((PHYSFS_File*)file->UserData, pos);
//		break;
//	case aiOrigin_CUR:
//		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_tell((PHYSFS_File*)file->UserData) + pos);
//		break;
//	case aiOrigin_END:
//		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_fileLength((PHYSFS_File*)file->UserData) + pos);
//		break;
//	}
//
//	if(res == 0)
//		LOG(LogType::L_ERROR, "File System error while SEEK via assimp: %s", PHYSFS_getLastError());
//
//	return (res != 0) ? aiReturn_SUCCESS : aiReturn_FAILURE;
//}

//aiFile* AssimpOpen(aiFileIO* io, const char* name, const char* format)
//{
//	static aiFile file;
//
//	file.UserData = (char*) PHYSFS_openRead(name);
//	file.ReadProc = AssimpRead;
//	file.WriteProc = AssimpWrite;
//	file.TellProc = AssimpTell;
//	file.FileSizeProc = AssimpSize;
//	file.FlushProc= AssimpFlush;
//	file.SeekProc = AssimpSeek;
//
//	return &file;
//}
//
//void AssimpClose(aiFileIO* io, aiFile* file)
//{
//	if (PHYSFS_close((PHYSFS_File*)file->UserData) == 0)
//		LOG(LogType::L_ERROR, "File System error while CLOSE via assimp: %s", PHYSFS_getLastError());
//}
//
//void FileSystem::CreateAssimpIO()
//{
//	RELEASE(assimpIO);
//
//	assimpIO = new aiFileIO;
//	assimpIO->OpenProc = AssimpOpen;
//	assimpIO->CloseProc = AssimpClose;
//}
//
//aiFileIO* FileSystem::GetAssimpIO()
//{
//	return assimpIO;
//}