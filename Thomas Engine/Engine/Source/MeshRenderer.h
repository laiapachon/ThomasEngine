#pragma once
#include "Component.h"

class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* obj);
	virtual ~MeshRenderer();

	void Update() override;
	void RenderMesh();
	void OnEditor() override;

	Mesh* mesh;
	bool faceNormals = false;
	bool vertexNormals = false;
};