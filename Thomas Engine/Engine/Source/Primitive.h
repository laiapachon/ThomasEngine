
#pragma once
#include "glmath.h"
#include "Color.h"
#include <vector>
#include "ResourceMesh.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();
	~Primitive(){};

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);

	void SetVertices(float vertices[], int size);
	void SetTexCoords(float texCoords[], int size);
	void SetIndices(int indices[], int size);

	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;

	Mesh* mesh;

protected:
	PrimitiveTypes type;
};

// ============================================
class PrimitiveCube : public Primitive
{
public :
	PrimitiveCube();
	PrimitiveCube(vec3 _size, vec3 pos);
	void InnerMesh();
public:
	vec3 size = { 0.5f, 0.5f, 0.5f };
};

// ============================================
class PrimitiveSphere : public Primitive
{
public:
	PrimitiveSphere();
	PrimitiveSphere(float radius, int sectors, int stacks);
	void InnerMesh();

private:
	void SetVerticesMesh();
	void SetIndicesMesh();

public:
	float radius = 1;
	int sectors = 36;
	int stacks = 18;
};

// ============================================
class PrimitiveCylinder : public Primitive
{
public:
	PrimitiveCylinder();
	PrimitiveCylinder(float radius, float height, int sectorCount);
	void InnerMesh();

private:
	std::vector<float> GetUnitCircleVertices();
	void SetVerticesMesh();
	void SetIndicesMesh();

public:
	float radius = 1;
	float height = 3;
	int sectorCount = 24;

private:
	int baseCenterIndex;
	int topCenterIndex;
};


class PrimitivePyramid : public Primitive
{
public:
	PrimitivePyramid();
	PrimitivePyramid(float radius, float height, int sectorCount);
	void InnerMesh();

public:
	float radius = 2;
	float height = 2;
};

// ============================================
class PrimitiveLine : public Primitive
{
public:
	PrimitiveLine();
	PrimitiveLine(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class PrimitivePlane : public Primitive
{
public:
	PrimitivePlane();
	PrimitivePlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};