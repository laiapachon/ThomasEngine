#pragma once
#include "GameObject.h"
#include "Glew/include/glew.h"
#include "Math/float3.h"

enum class ComponentType
{
	TRANSFORM,
	MESHRENDERER,
	MATERIAL,
	CAMERA
};

class Component
{
public:
	Component(GameObject* obj) { owner = obj; };
	virtual ~Component() { owner = nullptr; };

	virtual void Update() {};
	virtual void OnEditor() {};

	ComponentType GetType() { return type; };
	void SetType(ComponentType type) { this->type = type; };

	GameObject* GetOwner() { return owner; };

	void DrawBoundingBoxes(float3* points, float3 color = float3::one)
	{
		glColor3fv(&color.x);
		glLineWidth(2.f);
		glBegin(GL_LINES);

		for (int i = 0; i < 24; i++)
		{
			glVertex3fv(&points[index[i]].x);
		}

		glEnd();
		glLineWidth(1.f);
		glColor3f(1.f, 1.f, 1.f);
	}

	bool active = true;
private:

	int index[24] = { 0, 2, 2, 6, 6, 4, 4, 0,
					0, 1, 1, 3, 3, 2, 4, 5,
					6, 7, 5, 7, 3, 7, 1, 5 };

	ComponentType type;
	GameObject* owner = nullptr;
};