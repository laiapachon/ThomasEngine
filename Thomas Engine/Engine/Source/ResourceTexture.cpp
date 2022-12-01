#include "ResourceTexture.h"
#include "Globals.h"
#include "FileSystem.h"
#include "TextureLoader.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

Texture::Texture(std::string path, std::string name) : Resource(ResourceType::TEXTURE), path(path), name(name)
{
	SetAssetsPath(path.c_str());
	std::string libraryPath;
	if (path.find("/") != std::string::npos)
	{
		int pos = path.find_last_of("/");
		libraryPath = "Library/Textures" + path.substr(pos, path.find_last_of(".") - pos) + ".dds";
	}
	else
		libraryPath = "Library/Textures/" + path.substr(0, path.find_last_of(".")) + ".dds";
	SetLibraryPath(libraryPath.c_str());
}

bool Texture::LoadToMemory()
{
	LOG(LogType::L_NORMAL, "Texture loaded to memory");

	// Calculate buffer size
	char* buffer = nullptr;
	int bufferSize = FileSystem::FileLoad(path.c_str(), &buffer);

	// Generate new textureID
	textureID = TextureLoader::LoadTexOnMemory(buffer, bufferSize, &texWidth, &texHeight);

	RELEASE_ARRAY(buffer);

	return true;
}
// Free texture
bool Texture::UnloadFromMemory()
{
	if (textureID != 0)
		glDeleteTextures(1, &textureID);

	textureID = 0, texWidth = 0, texHeight = 0;

	return true;
}

void Texture::Import(char* buffer, int size, const char* name)
{
	bool dds = TextureLoader::SaveToDds(buffer, size, name);
	if (dds) path = name;
}