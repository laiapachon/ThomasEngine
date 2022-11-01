#include "ResourceMesh.h"
#include "Globals.h"

#include "Glew/include/glew.h"
#include "glmath.h"

Mesh::Mesh(unsigned int uid) : Resource(uid, ResourceType::MESH)
{
}

Mesh::~Mesh()
{
	//Clear buffers (Should not happen on Mesh components)
	if (indexBufferId != -1)
		glDeleteBuffers(1, &indexBufferId);

	if (vertexBufferId != -1)
		glDeleteBuffers(1, &vertexBufferId);

	if (textureBufferId != -1)
		glDeleteBuffers(1, &textureBufferId);

	if (normalBufferId != -1)
		glDeleteBuffers(1, &normalBufferId);

	//Clear buffers
	UnloadFromMemory();
}

bool Mesh::LoadToMemory()
{
	// Vertex Buffer GL_ARRAY_BUFFER
	if (numVertex != 0)
	{
		glGenBuffers(1, (GLuint*)&(vertexBufferId));
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertex * 3, &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//Index Buffer GL_ELEMENT_ARRAY_BUFFER
	if (numIndices != 0)
	{
		glGenBuffers(1, (GLuint*)&(indexBufferId));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * numIndices, &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Normals Buffer GL_ARRAY_BUFFER
	if (numNormals != 0)
	{
		glGenBuffers(1, (GLuint*)&(normalBufferId));
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numNormals * 3, &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// TexCoords Buffer GL_ARRAY_BUFFER
	if (numTexCoords != 0)
	{
		glGenBuffers(1, (GLuint*)&(textureBufferId));
		glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numTexCoords * 2, &texCoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return true;
}

bool Mesh::UnloadFromMemory()
{
	//Clear buffers
	indices.clear();
	vertices.clear();
	normals.clear();
	texCoords.clear();

	return true;
}

void Mesh::RenderMesh(GLuint textureID)
{
	if (textureID != -1)
		glBindTexture(GL_TEXTURE_2D, textureID);

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

	//-- Draw --//
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);

	//-- UnBind Buffers--//
	if (textureID != -1)
		glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/*if (numTexCoords != 0) glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	if (textureID != -1) glBindTexture(GL_TEXTURE_2D, 0);*/

	//--Disables States--//
	glDisableClientState(GL_VERTEX_ARRAY);
	if (numNormals != 0)
		glDisableClientState(GL_NORMAL_ARRAY);
	if (numTexCoords != 0)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Mesh::RenderMeshDebug(bool* vertexNormals, bool* faceNormals)
{
	if (*vertexNormals == true)
	{
		float normalLenght = 0.05f;
		glPointSize(3.0f);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < numVertex * 3; i += 3)
		{
			glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
		}
		glEnd();
		glColor3f(0, 1, 0);
		glPointSize(1.0f);

		//Vertex normals
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < numNormals * 3; i += 3)
		{
			glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
			glVertex3f(vertices[i] + normals[i] * normalLenght, vertices[i + 1] + normals[i + 1] * normalLenght, vertices[i + 2] + normals[i + 2] * normalLenght);
		}
		glEnd();
		glColor3f(1, 1, 1);
	}

	if (*faceNormals == true)
	{
		float normalLenght = 0.05f;
		//Face normals
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for (int i = 0; i < numIndices; i += 3)
		{
			vec3 A = GetVectorFromIndex(&vertices[indices[i] * 3]);

			vec3 B = GetVectorFromIndex(&vertices[indices[i + 1] * 3]);

			vec3 C = GetVectorFromIndex(&vertices[indices[i + 2] * 3]);

			vec3 middle((A.x + B.x + C.x) / 3.f, (A.y + B.y + C.y) / 3.f, (A.z + B.z + C.z) / 3.f);

			vec3 crossVec = cross((B - A), (C - A));
			vec3 normalDirection = normalize(crossVec);

			//LOG("%f, %f, %f", middle.x, middle.y, middle.z);
			glVertex3f(middle.x, middle.y, middle.z);
			glVertex3f(middle.x + normalDirection.x * normalLenght, middle.y + normalDirection.y * normalLenght, middle.z + normalDirection.z * normalLenght);
		}
		glEnd();
		glPointSize(1.f);
		glColor3f(1, 1, 1);
	}
}

vec3 Mesh::GetVectorFromIndex(float* startValue)
{
	float x = *startValue;
	++startValue;
	float y = *startValue;
	++startValue;
	float z = *startValue;

	return vec3(x, y, z);
}