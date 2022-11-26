#include "ResourceTexture.h"
#include "Globals.h"
#include "FileSystem.h"
#include "TextureLoader.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

Texture::Texture(std::string path, std::string name) : Resource(ResourceType::TEXTURE), path(path), name(name)
{
}

bool Texture::LoadToMemory()
{
	LOG(LogType::L_NORMAL, "Texture loaded to memory");

	// Calculate buffer size
	char* buffer = nullptr;
	int bufferSize = FileSystem::LoadToBuffer(path.c_str(), &buffer);

	// Generate new textureID
	textureID = TextureLoader::LoadToMemory(buffer, bufferSize, &texWidth, &texHeight);

	delete[] buffer;
	buffer = nullptr;

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