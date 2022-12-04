#include "ResourceMesh.h"
#include "Globals.h"

#include "Glew/include/glew.h"
#include "Geometry/Sphere.h"

Mesh::Mesh() : Resource(ResourceType::MESH)
{
}

Mesh::~Mesh()
	{
		CleanUp();
	}
	void Mesh::CleanUp()
	{

	if (indexBufferId != -1)
		glDeleteBuffers(1, &indexBufferId);

	if (vertexBufferId != -1)
		glDeleteBuffers(1, &vertexBufferId);

	if (textureBufferId != -1)
		glDeleteBuffers(1, &textureBufferId);

	if (normalBufferId != -1)
		glDeleteBuffers(1, &normalBufferId);


	UnloadFromMemory();
}

bool Mesh::LoadToMemory()
{
	if (numIndexs != 0)
	{
		glGenBuffers(1, (GLuint*)&(indexBufferId));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * numIndexs, &indexs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}	

	if (numVertex != 0)
	{
		glGenBuffers(1, (GLuint*)&(vertexBufferId));
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertex * 3, &vertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (numTexCoords != 0)
	{
		glGenBuffers(1, (GLuint*)&(textureBufferId));
		glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numTexCoords * 2, &texCoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}	

	if (numNormals != 0)
	{
		glGenBuffers(1, (GLuint*)&(normalBufferId));
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numNormals * 3, &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return true;
}

bool Mesh::UnloadFromMemory()
{	
	indexs.clear();
	vertex.clear();
	normals.clear();
	texCoords.clear();
	return true;
}

void Mesh::Render(GLuint textureID)
{
	if (textureID != -1) glBindTexture(GL_TEXTURE_2D, textureID);

	EnableClientState();

	glDrawElements(GL_TRIANGLES, numIndexs, GL_UNSIGNED_INT, NULL);
	
	UnBindBuffers(textureID);
	
	DisableClientState();
}

void Mesh::EnableClientState()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (numTexCoords != 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}
	if (numNormals != 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
}

void Mesh::DisableClientState()
{
	glDisableClientState(GL_VERTEX_ARRAY);
	if (numNormals != 0)
		glDisableClientState(GL_NORMAL_ARRAY);
	if (numTexCoords != 0)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Mesh::UnBindBuffers(const GLuint& textureID)
{
	if (textureID != -1)
		glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::DebugRender(bool* vertexNormals, bool* faceNormals)
{
	float normalLenght = 0.1f;

	if (*vertexNormals == true) RenderVertexNormals(normalLenght);

	if (*faceNormals == true) RenderFaceNormals(normalLenght);
}

void Mesh::RenderFaceNormals(float normalLenght)
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);

	meshABC abc;

	for (int i = 0; i < numIndexs; i += 3)
	{
		for (int j = 0; j < 3; j++)
			abc.vecABC[j] = GetIndexVec(&vertex[indexs[i + j] * 3]);

		abc.UpdateABC();
		
		vec3 central((abc.A.x + abc.B.x + abc.C.x) / 3.f, (abc.A.y + abc.B.y + abc.C.y) / 3.f, (abc.A.z + abc.B.z + abc.C.z) / 3.f);
		vec3 vCross = cross((abc.B - abc.A), (abc.C - abc.A));
		vec3 normalDirection = normalize(vCross);
		glVertex3f(central.x, central.y, central.z);
		glVertex3f(central.x + normalDirection.x * normalLenght, central.y + normalDirection.y * normalLenght, central.z + normalDirection.z * normalLenght);

	}
	glEnd();
	glPointSize(1.f);
	glColor3f(1, 1, 1);
}

void Mesh::RenderVertexNormals(float normalLenght)
{
	for (int type = 0; type <= 1; type++)
	{			
		glColor3f(1-type, 0+type, 0);
		(type == 0) ? glPointSize(3.0f), glBegin(GL_POINTS) :glBegin(GL_LINES);
		
		if(type==0)
		for (unsigned int i = 0; i < numVertex * 3; i += 3)
		{
			glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);

		}
		if(type==1)
		for (unsigned int i = 0; i < numNormals * 3; i += 3)
		{
			glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
			glVertex3f(vertex[i] + normals[i] * normalLenght, vertex[i + 1] + normals[i + 1] * normalLenght, vertex[i + 2] + normals[i + 2] * normalLenght);
		}

		glColor3f(0+type, 1, 0+type);
		if (type == 0) glPointSize(1.0f);
		glEnd();
	}
}

vec3 Mesh::GetIndexVec(float* startValue)
{
	float x = *startValue;
	float y = *(++startValue);
	float z = *(++startValue);

	return  vec3(x, y, z);
}

void Mesh::GenerateBounds()
{
	localAABB.SetNegativeInfinity();

	std::vector<float3> vertices;
	float3 pointArray;
	for (int i = 0; i < vertex.size(); i+=3)
	{
		pointArray = float3(vertex[i], vertex[i+1], vertex[i+2]);
		vertices.push_back(pointArray);
	}
	
	localAABB.Enclose(&vertices[0], vertices.size());
		
	Sphere sphere;	
	sphere.r = 0.f;
	sphere.pos = localAABB.CenterPoint();
	sphere.Enclose(localAABB);

	radius = sphere.r;
	centerPoint = sphere.pos;

	vertices.clear();
}
