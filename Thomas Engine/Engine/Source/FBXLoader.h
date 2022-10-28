#pragma once

#include "Mesh.h"

class FBXLoader {
public:
	FBXLoader();

	~FBXLoader();

	void LoadMesh(const char* path);

	inline Mesh& GetMesh() { return ourMesh; }

private:
	Mesh ourMesh;
};
