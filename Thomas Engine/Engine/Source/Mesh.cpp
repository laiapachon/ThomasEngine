#include "Mesh.h"

#include "Glew/include/glew.h"

Mesh::Mesh()
{
}


void Mesh::SetupMesh()
{
	// Vertex Buffer GL_ARRAY_BUFFER
	glGenBuffers(1, (uint*)&(vertexBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, &vertices[0], GL_STATIC_DRAW);

	//Index Buffer GL_ELEMENT_ARRAY_BUFFER
	glGenBuffers(1, (uint*)&(indexBufferId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void Mesh::SetVertices(float _vertices[], int size)
{
	for (int i = 0; i < size; i++)
	{
		vertices.push_back(_vertices[i]);
	}
}

void Mesh::SetIndices(int _indices[], int size)
{
	for (int i = 0; i < size; i++)
	{
		indices.push_back(_indices[i]);
	}
}

void Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//-- Buffers--//
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	/*glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, textureId);
	*/
	//-- Draw --//

	glPushMatrix();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	glPopMatrix();
	//-- UnBind Buffers--//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	//--Disables States--//
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}