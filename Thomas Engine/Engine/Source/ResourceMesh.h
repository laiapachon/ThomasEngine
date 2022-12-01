#pragma once
#include "Resource.h"

#include <vector>
#include "glmath.h"
#include "Geometry/AABB.h"

typedef unsigned int uint;
typedef unsigned int GLuint;

struct meshABC {

public:
	vec3 vecABC[3] = {  vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0) };
	vec3 A = vecABC[0];
	vec3 B = vecABC[1];
	vec3 C = vecABC[2];

	void UpdateABC() 
	{
		A = vecABC[0];
		B = vecABC[1];
		C = vecABC[2];
	}
};


class Mesh : public Resource
{
public:
	Mesh();
	~Mesh();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void Render(GLuint textureID = -1);

	void EnableClientState();
	void DisableClientState();

	void UnBindBuffers(const GLuint& textureID);

	void DebugRender(bool* vertexNormals, bool* faceNormals);
	void RenderFaceNormals(float normalLenght);
	void RenderVertexNormals(float normalLenght);

	vec3 GetIndexVec(float* startValue);
	void GenerateBounds();

	void CleanUp();
public:
	uint indexBufferId = 0;
	uint vertexBufferId = 0;
	uint textureBufferId = 0;
	uint normalBufferId = 0;

	uint numIndexs = 0;
	uint numVertex = 0;
	uint numTexCoords = 0;
	uint numNormals = 0;

	std::vector<uint> indexs;
	std::vector<float> vertex;
	std::vector<float> texCoords;
	std::vector<float> normals;

	float3 centerPoint = float3::zero;
	float radius;
	AABB localAABB;
};