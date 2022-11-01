#include "Application.h"
#include "FileSystem.h"
#include "Globals.h"

#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "MeshLoader.h"

// This pragma comment shouldn't be necessary because it is already included in additional directories but if not give fail 
//#pragma comment( lib, "DevIL/libx86/ILUT.lib" )

// File System Init
void FileSystem::FSInit()
{
	// In future this should be move to ResourceManager (the CleanUp, also)
	// Devil init
	LOG(LogType::L_NORMAL, "DevIL Init");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	MeshLoader::EnableDebugMode();

	// PHYSFS_init
	// Needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	//Setting the working directory as the writing directory
	if (PHYSFS_setWriteDir(".") == 0)
		LOG(LogType::L_NORMAL, "File System error while creating write dir: %s\n", PHYSFS_getLastError());

	// Adding ProjectFolder (working directory)
	FileSystem::AddPath(".");
	// Adding AssestsFolder
	std::string assetPath = GetBasePath();
	assetPath += ASSETS_FOLDER;
	assetPath = NormalizePath(assetPath.c_str());
	FileSystem::AddPath(assetPath.c_str());

	// Dump list of paths
	LOG(LogType::L_NORMAL, "FileSystem Operations base is [%s] plus:", GetBasePath());
	LOG(LogType::L_NORMAL, GetReadPaths());

	FileSystem::CreateLibraryDirectories();
}


// Extract file name, from last "/" until the "."
std::string StringLogic::FileNameFromPath(const char* _path)
{
	std::string fileName(_path);

	fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.'));

	return fileName;
}
// Convert global path to local path, example:
// C:\Users\Enric\OneDrive\Escritorio\Universidad\3r Carrera\Motores\Fire-Engine\Fire_Engine\Output\Assests\BakerHouse.fbx
// to: Assests\BakerHouse.fbx
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
// Get file type from path according to its extension 
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

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

void FileSystem::FSDeInit()
{
	PHYSFS_deinit();
	MeshLoader::DisableDebugMode();
}
// If don't exist this paths, let's create 
void FileSystem::CreateLibraryDirectories()
{
	CreateDir(LIBRARY_FOLDER);
	CreateDir(MESHES_FOLDER);
	CreateDir(TEXTURES_FOLDER);
	CreateDir(MATERIALS_FOLDER);
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
// Check if a file exists
bool FileSystem::Exists(const char* file)
{
	return PHYSFS_exists(file) != 0;
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
// Return all existing paths
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
// Check if a file is a directory
bool FileSystem::IsDirectory(const char* file)
{
	return PHYSFS_isDirectory(file) != 0;
}
// Substitute "\\" to "/"
std::string FileSystem::NormalizePath(const char* full_path)
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}
std::string FileSystem::UnNormalizePath(const char* full_path)
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '/')
			newPath[i] = '\\';
	}
	return newPath;
}
// Split file path
void FileSystem::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension)
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

unsigned int FileSystem::Load(const char* path, const char* file, char** buffer)
{
	std::string full_path(path);
	full_path += file;
	return LoadToBuffer(full_path.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint FileSystem::LoadToBuffer(const char* file, char** buffer)
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

// Duplicate a file to a local PhysFS valid path
unsigned int FileSystem::Copy(const char* file, const char* dir, std::string& outputFile)
{
	uint size = 0;

	if (file == nullptr || dir == nullptr)
		return size;

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

bool FileSystem::Remove(const char* file)
{
	bool ret = false;

	if (file != nullptr)
	{
		if (PHYSFS_delete(file) != 0)
		{
			LOG(LogType::L_NORMAL, "File deleted: [%s]", file);
			ret = true;
		}
		else
			LOG(LogType::L_ERROR, "File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError());
	}

	return ret;
}

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

void FileSystem::OnGui()
{
	if (ImGui::CollapsingHeader("FileSystem"))
	{
		IMGUI_PRINT("Base Path: ", GetBasePath());
		IMGUI_PRINT("Read Paths: ", GetReadPaths());
		IMGUI_PRINT("Write Path: ", GetWritePath());
	}
}