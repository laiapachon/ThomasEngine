#include "ResourceMesh.h"

#include "Glew/include/glew.h"

Mesh::Mesh(unsigned int _uid) : Resource(_uid, Resource::Type::MESH)
{
}

Mesh::~Mesh()
{
}

bool Mesh::LoadToMemory()
{
	// Vertex Buffer GL_ARRAY_BUFFER
	glGenBuffers(1, (uint*)&(vertexBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, &vertices[0], GL_STATIC_DRAW);

	//Index Buffer GL_ELEMENT_ARRAY_BUFFER
	glGenBuffers(1, (uint*)&(indexBufferId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// TexCoords Buffer GL_ARRAY_BUFFER
	glGenBuffers(1, (uint*)&(textureBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords.size() * 2, &texCoords[0], GL_STATIC_DRAW);

	//texcoords attribute
	/*glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * texCoords.size() * 2, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);*/

	return true;
}

bool Mesh::UnloadFromMemory()
{
	//Clear buffers
	indices.clear();
	vertices.clear();

	return true;
}

void Mesh::SetVertices(float _vertices[], int size)
{
	for (int i = 0; i < size; i++)
	{
		vertices.push_back(_vertices[i]);
	}
}

void Mesh::SetTexCoords(float _texCoords[], int size)
{
	for (int i = 0; i < size; i++)
	{
		texCoords.push_back(_texCoords[i]);
	}
}

void Mesh::SetIndices(int _indices[], int size)
{
	for (int i = 0; i < size; i++)
	{
		indices.push_back(_indices[i]);
	}
}

void Mesh::RenderMesh()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//-- Buffers--//
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	//glBindTexture(GL_TEXTURE_2D, textureId);
	
	//-- Draw --//

	glPushMatrix();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	glPopMatrix();
	//-- UnBind Buffers--//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//--Disables States--//
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Mesh::CleanUp()
{
	//Clear buffers
	indices.clear();
	vertices.clear();
}
