#include "TextureLoader.h"
#include "FileSystem.h"
#include"Globals.h"

#include "Glew/include/glew.h"

#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#include"Resource.h"

bool TextureLoader::SaveToDds(char* buffer, int size, const char* name)
{
	ILuint textureID;
	ilGenImages(1, &textureID);
	ilBindImage(textureID);

	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG(LogType::L_ERROR, "Error loading texture");
		ilDeleteImages(1, &textureID);
		return false;
	}

	ILuint newSize;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	newSize = ilSaveL(IL_DDS, nullptr, 0); 
	if (newSize > 0) {
		data = new ILubyte[newSize]; 
		if (ilSaveL(IL_DDS, data, newSize) > 0) 
			buffer = (char*)data;

		FileSystem::FileSave(name, buffer, newSize);
		RELEASE_ARRAY(data);
	}
	ilDeleteImages(1, &textureID);
	return true;
}
GLuint TextureLoader::LoadTexOnMemory(char* buffer, int size, int* w, int* h)
{
	ILuint textureID;
	ilGenImages(1, &textureID);
	ilBindImage(textureID);

	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG(LogType::L_ERROR, "Error loading texture on memory");
	}
	if (w)
		*w = ilGetInteger(IL_IMAGE_WIDTH);
	if (h)
		*h = ilGetInteger(IL_IMAGE_HEIGHT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint glID = ilutGLBindTexImage();

	glBindTexture(GL_TEXTURE_2D, glID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	ilDeleteImages(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, 0);

	return glID;
}

