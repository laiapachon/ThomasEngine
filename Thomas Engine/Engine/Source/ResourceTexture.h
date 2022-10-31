#pragma once
#include"Resource.h"
#include"Color.h"

struct Rect
{
	int x, y, w, h;
	Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
	Rect() : x(0), y(0), w(0), h(0) {}
	bool Contains(Rect b);
};

typedef unsigned int GLuint;


class Texture : public Resource
{
public:
	Texture(unsigned int textureID);
	Texture(GLuint textureID, int texWidth, int texHeight, const char* name = nullptr, const char* path = nullptr);
	~Texture();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;
	Rect GetTextureChunk(Rect area);

public:
	int texWidth;
	int texHeight;

	GLuint textureID;
	Color color;

	const char* path;
	const char* name;
};
