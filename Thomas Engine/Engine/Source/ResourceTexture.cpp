#include "ResourceTexture.h"
#include "Globals.h"
#include "FileSystem.h"
#include "TextureLoader.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

bool Rect::Contains(Rect b)
{
    if ((b.x + b.w) <= (x + w)
        && (b.x) >= (x)
        && (b.y) >= (y)
        && (b.y + b.h) <= (y + h)
        )
    {
        return true;
    }
    else
    {
        return false;
    }
}

Texture::Texture(unsigned int _uid) : Resource(_uid, Resource::Type::TEXTURE), textureID(0), tWidth(0), tHeight(0)
{
	color = White;
}

Texture::~Texture()
{
}

bool Texture::LoadToMemory()
{
	LOG(LogType::L_WARNING, "Texture loaded to memory");
	char* buffer = nullptr;
	int size = FileSystem::LoadToBuffer(GetLibraryPath(), &buffer);

	textureID = TextureLoader::LoadToMemory(buffer, size, &tWidth, &tHeight);

	delete[] buffer;
	buffer = nullptr;

	return true;
}

bool Texture::UnloadFromMemory()
{
	if (textureID != 0)
		glDeleteTextures(1, &textureID);

	textureID = 0;
	tWidth = 0;
	tHeight = 0;

	return true;
}

Rect Texture::GetTextureChunk(Rect area)
{
	//Rect of the chunk we need to get
	Rect ret;
	if (!Rect(0, 0, this->tWidth, this->tHeight).Contains(area))
		return ret;

	//Calculate rect as normalized rect
	ret = area;

	ret.x = ret.x / tWidth;
	ret.w = (ret.x + ret.w) / tWidth;

	ret.y = ret.y / tHeight;
	ret.h = (ret.y + ret.h) / tHeight;

	//Position should be inside the 0, 1 range and position + width (normalized) should not be more than 1
	return ret;
}
