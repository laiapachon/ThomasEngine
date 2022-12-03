#include "Application.h"
#include "FileSystem.h"
#include "Globals.h"

#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "MeshLoader.h"
#include "Editor.h"

// File System Init
void FileSystem::FSInit()
{
	// Devil init
	LOG(LogType::L_NORMAL, "DevIL Init");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	MeshLoader::EnableDebugMode();
	
	LOG(LogType::L_NORMAL, "PHYSFS Init");
	char* basePath = SDL_GetBasePath();
	PHYSFS_init(basePath);
	SDL_free(basePath);

	
	if (PHYSFS_setWriteDir(basePath) == 0)
		LOG(LogType::L_NORMAL, "File System error while creating write dir: %s\n", PHYSFS_getLastError());

	
	std::string assetPath = GetBasePath();
	assetPath = ExtractLocalDiskBackward(assetPath.c_str());
	FileSystem::AddPath(assetPath.c_str());

	
	assetPath = GetBasePath();
	assetPath = NormalizePath(assetPath.c_str());
	assetPath += ASSETS_FOLDER;
	FileSystem::AddPath(assetPath.c_str());

	
	assetPath = GetBasePath();
	assetPath = NormalizePath(assetPath.c_str());
	//assetPath += LIBRARY_FOLDER;
	FileSystem::AddPath(assetPath.c_str());

	
	LOG(LogType::L_NORMAL, "Get Base Path: ");
	LOG(LogType::L_NORMAL, GetBasePath());
	LOG(LogType::L_NORMAL, "Get Read Path: ");
	LOG(LogType::L_NORMAL, GetReadPaths());
	LOG(LogType::L_NORMAL, "Get Write Path: ");
	LOG(LogType::L_NORMAL, GetWritePath());

	FileSystem::LibraryMaker();
}

std::string StringLogic::FileNameFromPath(const char* path)
{
	std::string fileName(path);

	fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.'));

	return fileName;
}

std::string StringLogic::GlobalToLocalPath(const char* globalPath)
{
	std::string localPath = FileSystem::NormalizePath(globalPath);

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

ImportType FileSystem::GetImportType(const char* path)
{
	std::string ext(path);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx" || ext == "obj" || ext == "stl" || ext == "skp" || ext == "gltf" || ext == "glb" || ext == "usd")
		return ImportType::MESH;
	if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "dds" || ext == "bmp" || ext == "tga")
		return ImportType::TEXTURE;

	return ImportType::NOTYPE;
}

void FileSystem::FSDeInit()
{
	PHYSFS_deinit();
	MeshLoader::DisableDebugMode();
}

void FileSystem::LibraryMaker()
{
	CreateDir(LIBRARY_FOLDER);
	CreateDir(MESHES_FOLDER);
	CreateDir(TEXTURES_FOLDER);
	CreateDir(MATERIALS_FOLDER);
}
bool FileSystem::CreateDir(const char* dir)
{
	if (IsDirFound(dir) == false)
	{
		PHYSFS_mkdir(dir);
		return true;
	}
	return true;
}

bool FileSystem::FileExist(const char* file)
{
	return PHYSFS_exists(file) != 0;
}


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

bool FileSystem::IsDirFound(const char* file)
{
	return PHYSFS_isDirectory(file) != 0;
}

std::string FileSystem::NormalizePath(const char* fullPath)
{
	std::string newPath(fullPath);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}
std::string FileSystem::UnNormalizePath(const char* fullPath)
{
	std::string newPath(fullPath);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '/')
			newPath[i] = '\\';
	}
	return newPath;
}

std::string FileSystem::ExtractLocalDiskBackward(const char* fullPath)
{
	std::string path = NormalizePath(fullPath);
	path = path.substr(0, path.find_first_of('/') + 1);
	return path;
}

std::string FileSystem::ExtractLocalDiskForward(const char* fullPath)
{
	std::string path = NormalizePath(fullPath);
	path = path.substr(path.find_first_of("/\\") + 1);
	return path;
}


uint FileSystem::FileLoad(const char* file, char** buffer)
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);

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

uint FileSystem::FileSave(const char* file, const void* buffer, unsigned int size, bool append)
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size) {
			LOG(LogType::L_ERROR, "File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true) {
				LOG(LogType::L_NORMAL, "Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), file);
			}
			
			else if (overwrite == false)
				LOG(LogType::L_NORMAL, "New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), file, size);

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG(LogType::L_ERROR, "File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else {
		LOG(LogType::L_ERROR, "File System error while opening file %s: %s", file, PHYSFS_getLastError());
	}

	return ret;
}

bool FileSystem::FileRemove(const char* file)
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