#pragma once

#include"Resource.h"
#include<string>
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
	Texture(unsigned int _uid);
	~Texture();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;
	Rect GetTextureChunk(Rect area);

public:
	int tWidth;
	int tHeight;

	GLuint textureID;
	Color color;

};
