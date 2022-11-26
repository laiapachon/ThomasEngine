#pragma once

typedef unsigned int GLuint;
class Resource;

namespace TextureLoader
{
	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
};

