#pragma once
#include"Resource.h"
#include"Color.h"

typedef unsigned int GLuint;

class Texture : public Resource
{
public:
	Texture(std::string path, std::string name = "");
	~Texture() {};

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

public:
	int texWidth = 0;
	int texHeight = 0;

	GLuint textureID = -1;

	std::string path;
	std::string name;
};
