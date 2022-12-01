#pragma once

typedef unsigned int GLuint;
typedef unsigned int ILuint;
class Resource;

namespace TextureLoader
{
	bool SaveToDds(char* buffer, int size, const char* name);
	GLuint LoadTexOnMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
};

