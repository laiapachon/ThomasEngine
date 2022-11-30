#pragma once
#include "Component.h"
#include "Math/float3.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* obj);
	virtual ~MeshRenderer();

	void Update() override;
	void OnEditor() override;

	void RenderMesh();

	Mesh* GetMesh() { return mesh; };
	void SetMesh(Mesh* mesh) { this->mesh = mesh; };
	void SetBoundingBoxes(Mesh* mesh);

	float3 GetCenterPointInWorldCoords();
	float GetSphereRadius();

	AABB globalAABB;
	OBB globalOBB;

private:
	Mesh* mesh = nullptr;

	bool faceNormals = false;
	bool vertexNormals = false;
	bool showAABB = false;
	bool showOBB = false;
};