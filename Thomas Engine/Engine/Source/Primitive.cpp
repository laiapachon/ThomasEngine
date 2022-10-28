#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include <cmath>

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

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

// CUBE ============================================
Cube::Cube() : Primitive()
{
	type = PrimitiveTypes::Primitive_Cube;
}

Cube::Cube(vec3 size, vec3 pos) : Primitive(), size(size)
{
	type = PrimitiveTypes::Primitive_Cube;
	glTranslatef(pos.x, pos.y, pos.z);
}

void Cube::InnerMesh()
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
	SetVertices(vertex, 24);
	SetIndices(index, 36);
}



// SPHERE ============================================
Sphere::Sphere() : Primitive()
{
	type = PrimitiveTypes::Primitive_Sphere;
}

Sphere::Sphere(float radius, int sectors, int stacks) : Primitive(), radius(radius), sectors(sectors), stacks(stacks)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void Sphere::InnerMesh()
{
	SetVerticesMesh();
	SetIndicesMesh();
}

void Sphere::SetVerticesMesh()
{
	// clear memory of prev arrays
	std::vector<float>().swap(vertices);
	//std::vector<float>().swap(texCoords);

	float x, y, z, xz;                              // vertex position
	//float s, t;                                     // vertex texCoord

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
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// vertex tex coord (s, t) range between [0, 1]
			/*s = (float)j / sectorCount;
			t = (float)i / stackCount;
			texCoords.push_back(s);
			texCoords.push_back(t);*/
		}
	}
}

void Sphere::SetIndicesMesh()
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
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				indices.push_back(k2 + 1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
		}
	}
}



// CYLINDER ============================================
Cylinder::Cylinder() : Primitive()
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

Cylinder::Cylinder(float radius, float height, int sectorCount) : Primitive(), radius(radius), height(height), sectorCount(sectorCount)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void Cylinder::InnerMesh()
{
	SetVerticesMesh();
	SetIndicesMesh();
}

std::vector<float> Cylinder::GetUnitCircleVertices()
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

void Cylinder::SetVerticesMesh()
{
	// clear memory of prev arrays
	std::vector<float>().swap(vertices);
	//std::vector<float>().swap(texCoords);

	// get unit circle vectors on XY-plane
	std::vector<float> unitVertices = GetUnitCircleVertices();

	// put side vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // y value; -h/2 to h/2
		//float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uz = unitVertices[k + 2];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(h);                       // vy
			vertices.push_back(uz * radius);             // vz
		}
	}

	// the starting index for the base/top surface
	//NOTE: it is used for generating indices later
	baseCenterIndex = (int)vertices.size() / 3;
	topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

	// put base and top vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // y value; -h/2 to h/2

		// center point
		vertices.push_back(0);     vertices.push_back(h);     vertices.push_back(0);

		for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uz = unitVertices[k + 2];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(h);                       // vz
			vertices.push_back(uz * radius);             // vy
			// texture coordinate
			//texCoords.push_back(-ux * 0.5f + 0.5f);      // s
			//texCoords.push_back(-uy * 0.5f + 0.5f);      // t
		}
	}
}

void Cylinder::SetIndicesMesh()
{
	// generate CCW index list of cylinder triangles
	int k2 = 0;                         // 1st vertex index at base
	int k1 = sectorCount + 1;           // 1st vertex index at top

	// indices for the side surface
	for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
	{
		// 2 triangles per sector
		// k1 => k1+1 => k2
		indices.push_back(k1);
		indices.push_back(k1 + 1);
		indices.push_back(k2);

		// k2 => k1+1 => k2+1
		indices.push_back(k2);
		indices.push_back(k1 + 1);
		indices.push_back(k2 + 1);
	}

	// indices for the base surface
	//NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
	//      please see the previous code snippet
	for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(k);
			indices.push_back(k + 1);
			indices.push_back(baseCenterIndex);
		}
		else // last triangle
		{
			indices.push_back(k);
			indices.push_back(baseCenterIndex + 1);
			indices.push_back(baseCenterIndex);
		}
	}

	// indices for the top surface
	for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(k + 1);
			indices.push_back(k);
			indices.push_back(topCenterIndex);
		}
		else // last triangle
		{
			indices.push_back(topCenterIndex + 1);
			indices.push_back(k);
			indices.push_back(topCenterIndex);
		}
	}
}



// PYRAMID ============================================
Pyramid::Pyramid() : Primitive()
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

Pyramid::Pyramid(float radius, float height, int sectorCount) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void Pyramid::InnerMesh()
{
	SetVerticesMesh();
	SetIndicesMesh();
}

void Pyramid::SetVerticesMesh()
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
	SetVertices(vertex, 24);
}

void Pyramid::SetIndicesMesh()
{
	int index[] =
	{
		0, 2, 4,
		2, 3, 4,
		3, 1, 4,
		1, 0, 4,
		0, 1, 3,
		0, 3, 2,
	};
	SetIndices(index, 36);
}



// LINE ==================================================
Line::Line() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

Line::Line(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
Plane::Plane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

Plane::Plane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void Plane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}