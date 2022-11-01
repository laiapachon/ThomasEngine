#include "Application.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include <cmath>

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{
	mesh = new Mesh(app->GetNewUID());
}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3& u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// Set vertex, texCoords and index
void Primitive::SetVertices(float vertices[], int size)
{
	for (int i = 0; i < size; i++)
	{
		mesh->vertices.push_back(vertices[i]);
	}
	mesh->numVertex = mesh->vertices.size() / 3;
}

void Primitive::SetTexCoords(float texCoords[], int size)
{
	for (int i = 0; i < size; i++)
	{
		mesh->texCoords.push_back(texCoords[i]);
	}
	mesh->numTexCoords = mesh->texCoords.size() / 2;
}

void Primitive::SetIndices(int indices[], int size)
{
	for (int i = 0; i < size; i++)
	{
		mesh->indices.push_back(indices[i]);
	}
	mesh->numIndices = mesh->indices.size();
}

// CUBE ============================================
PrimitiveCube::PrimitiveCube() : Primitive()
{
	type = PrimitiveTypes::Primitive_Cube;
}

PrimitiveCube::PrimitiveCube(vec3 size, vec3 pos) : Primitive(), size(size)
{
	type = PrimitiveTypes::Primitive_Cube;
	glTranslatef(pos.x, pos.y, pos.z);
}

void PrimitiveCube::InnerMesh()
{
	float vertex[] =
	{
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f
	};
	for (int i = 0; i < 24; i += 3)
	{
		vertex[i] *= size.x;
		vertex[i + 1] *= size.y;
		vertex[i + 2] *= size.z;
	}
	int index[] =
	{
		0, 1, 2, 2, 3, 0,
		3, 2, 6, 6, 7, 3,
		7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4,
		5, 1, 0, 0, 4, 5,
		1, 5, 6, 6, 2, 1
	};
	float texCoords[] =
	{
		1.0000, 0.0000, 0.0000,
		1.0000, 1.0000, 0.0000,
		0.0000, 1.0000, 0.0000,
		0.0000, 0.0000, 0.0000
	};

	SetVertices(vertex, 24);
	SetIndices(index, 36);
	SetTexCoords(texCoords, 12);
}

// SPHERE ============================================
PrimitiveSphere::PrimitiveSphere() : Primitive()
{
	type = PrimitiveTypes::Primitive_Sphere;
}

PrimitiveSphere::PrimitiveSphere(float radius, int sectors, int stacks) : Primitive(), radius(radius), sectors(sectors), stacks(stacks)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void PrimitiveSphere::InnerMesh()
{
	SetVerticesMesh();
	SetIndicesMesh();
}

void PrimitiveSphere::SetVerticesMesh()
{
	// clear memory of prev arrays
	std::vector<float>().swap(mesh->vertices);
	std::vector<float>().swap(mesh->texCoords);

	float x, y, z, xz;                              // vertex position
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectors;
	float stackStep = PI / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xz = radius * cosf(stackAngle);             // r * cos(u)
		y = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
			z = xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
			mesh->vertices.push_back(x);
			mesh->vertices.push_back(y);
			mesh->vertices.push_back(z);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectors;
			t = (float)i / stacks;
			mesh->texCoords.push_back(s);
			mesh->texCoords.push_back(t);
		}
	}
	mesh->numVertex = mesh->vertices.size() / 3;
	mesh->numTexCoords = mesh->texCoords.size() / 2;
}

void PrimitiveSphere::SetIndicesMesh()
{
	// generate CCW index list of sphere triangles
	// k1--k1+1
	// |  / |
	// | /  |
	// k2--k2+1
	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				mesh->indices.push_back(k1 + 1);
				mesh->indices.push_back(k2);
				mesh->indices.push_back(k1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				mesh->indices.push_back(k2 + 1);
				mesh->indices.push_back(k2);
				mesh->indices.push_back(k1 + 1);
			}
		}
	}
	mesh->numIndices = mesh->indices.size();
}



// CYLINDER ============================================
PrimitiveCylinder::PrimitiveCylinder() : Primitive()
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

PrimitiveCylinder::PrimitiveCylinder(float radius, float height, int sectorCount) : Primitive(), radius(radius), height(height), sectorCount(sectorCount)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void PrimitiveCylinder::InnerMesh()
{
	SetVerticesMesh();
	SetIndicesMesh();
}

std::vector<float> PrimitiveCylinder::GetUnitCircleVertices()
{
	float sectorStep = 2 * PI / sectorCount;
	float sectorAngle;  // radian

	std::vector<float> unitCircleVertices;
	for (int i = 0; i <= sectorCount; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(cos(sectorAngle)); // x
		unitCircleVertices.push_back(0);                // y
		unitCircleVertices.push_back(sin(sectorAngle)); // z
	}
	return unitCircleVertices;
}

void PrimitiveCylinder::SetVerticesMesh()
{
	// clear memory of prev arrays
	std::vector<float>().swap(mesh->vertices);
	std::vector<float>().swap(mesh->texCoords);

	// get unit circle vectors on XY-plane
	std::vector<float> unitVertices = GetUnitCircleVertices();

	// put side vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // y value; -h/2 to h/2
		float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uz = unitVertices[k + 2];
			// position vector
			mesh->vertices.push_back(ux * radius);             // vx
			mesh->vertices.push_back(h);                       // vy
			mesh->vertices.push_back(uz * radius);             // vz
		}
	}

	// the starting index for the base/top surface
	//NOTE: it is used for generating indices later
	baseCenterIndex = (int)mesh->vertices.size() / 3;
	topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

	// put base and top vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // y value; -h/2 to h/2

		// center point
		mesh->vertices.push_back(0);     mesh->vertices.push_back(h);     mesh->vertices.push_back(0);

		for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uz = unitVertices[k + 2];
			// position vector
			mesh->vertices.push_back(ux * radius);             // vx
			mesh->vertices.push_back(h);                       // vz
			mesh->vertices.push_back(uz * radius);             // vy
			// texture coordinate
			mesh->texCoords.push_back(-ux * 0.5f + 0.5f);      // s
			mesh->texCoords.push_back(-uz * 0.5f + 0.5f);      // t
		}
	}
	mesh->numVertex = mesh->vertices.size() / 3;
	mesh->numTexCoords = mesh->texCoords.size() / 2;
}

void PrimitiveCylinder::SetIndicesMesh()
{
	// generate CCW index list of cylinder triangles
	int k2 = 0;                         // 1st vertex index at base
	int k1 = sectorCount + 1;           // 1st vertex index at top

	// indices for the side surface
	for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
	{
		// 2 triangles per sector
		// k1 => k1+1 => k2
		mesh->indices.push_back(k1);
		mesh->indices.push_back(k1 + 1);
		mesh->indices.push_back(k2);

		// k2 => k1+1 => k2+1
		mesh->indices.push_back(k2);
		mesh->indices.push_back(k1 + 1);
		mesh->indices.push_back(k2 + 1);
	}

	// indices for the base surface
	//NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
	//      please see the previous code snippet
	for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			mesh->indices.push_back(k);
			mesh->indices.push_back(k + 1);
			mesh->indices.push_back(baseCenterIndex);
		}
		else // last triangle
		{
			mesh->indices.push_back(k);
			mesh->indices.push_back(baseCenterIndex + 1);
			mesh->indices.push_back(baseCenterIndex);
		}
	}

	// indices for the top surface
	for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			mesh->indices.push_back(k + 1);
			mesh->indices.push_back(k);
			mesh->indices.push_back(topCenterIndex);
		}
		else // last triangle
		{
			mesh->indices.push_back(topCenterIndex + 1);
			mesh->indices.push_back(k);
			mesh->indices.push_back(topCenterIndex);
		}
	}
	mesh->numIndices = mesh->indices.size();
}



// PYRAMID ============================================
PrimitivePyramid::PrimitivePyramid() : Primitive()
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

PrimitivePyramid::PrimitivePyramid(float radius, float height, int sectorCount) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void PrimitivePyramid::InnerMesh()
{
	float vertex[] =
	{
		-0.5, 0, -0.5,
		0.5f, 0, -0.5,
		-0.5, 0, 0.5f,
		0.5f, 0, 0.5f,
		0, height / radius, 0,
	};
	for (int i = 0; i < 15; i++)
	{
		vertex[i] *= radius;
	}
	int index[] =
	{
		0, 2, 4,
		2, 3, 4,
		3, 1, 4,
		1, 0, 4,
		0, 1, 3,
		0, 3, 2,
	};
	float texCoords[] =
	{
		0.5000, 0.1910,
		0.1910, 0.5000,
		0.5000, 0.8090,
		0.5000, 0.1910,
		0.5000, 0.8090,
		0.8090, 0.5000,

		0.5000, 0.1910,
		0.8090, 0.5000,
		1.0000, 0.0000,

		0.8090, 0.5000,
		0.5000, 0.8090,
		1.0000, 1.0000,

		0.5000, 0.8090,
		0.1910, 0.5000,
		0.0000, 1.0000,

		0.1910, 0.5000,
		0.5000, 0.1910,
		0.0000, 0.0000,
	};

	SetVertices(vertex, 15);
	SetIndices(index, 18);
	SetTexCoords(texCoords, 36);
}


// LINE ==================================================
PrimitiveLine::PrimitiveLine() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

PrimitiveLine::PrimitiveLine(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void PrimitiveLine::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
PrimitivePlane::PrimitivePlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

PrimitivePlane::PrimitivePlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void PrimitivePlane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 50.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}