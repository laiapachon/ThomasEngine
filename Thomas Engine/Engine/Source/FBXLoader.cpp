
#include "Application.h"
#include "FBXLoader.h"
#include "Renderer3D.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "GPUDetected/DeviceId.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "assimp.lib")

Loader::Loader(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}
// Destructor
	Loader::~Loader()
{}

// Called before render is available
bool Loader::Init()
{

	bool ret = true;
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	
	path = "Assets/BakerHouse.fbx";
	LoadFile(path);
	

	return ret;
}

// PreUpdate: clear buffer
update_status Loader::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status Loader::PostUpdate(float dt)
{
	update_status ret;
	
	return UPDATE_CONTINUE;
}
void Loader::LoadFile(string path)
{
	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		
		for (int i = 0; i < scene->mNumMeshes; i++) {

			Mesh* ourMesh = new Mesh();

			// copy vertices
			ourMesh->num_vertices = scene->mMeshes[i]->mNumVertices;
			ourMesh->vertices = new float[ourMesh->num_vertices * 3];

			//memcpy(ourMesh->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * ourMesh->num_vertices * 3);
			for (int k = 0; k < ourMesh->num_vertices; k++) {

				ourMesh->vertices[k * VERTICES] = scene->mMeshes[i]->mVertices[k].x;
				ourMesh->vertices[k * VERTICES + 1] = scene->mMeshes[i]->mVertices[k].y;
				ourMesh->vertices[k * VERTICES + 2] = scene->mMeshes[i]->mVertices[k].z;
							

			}

			LOG(LogType::L_NORMAL, "New mesh with % d vertices", ourMesh->num_vertices);

			// copy faces
			if (scene->mMeshes[i]->HasFaces())
			{
				ourMesh->num_indices = scene->mMeshes[i]->mNumFaces * 3;
				ourMesh->indices = new uint[ourMesh->num_indices]; // assume each face is a triangle

				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3) {
						LOG(LogType::L_ERROR, "WARNING, geometry face with != 3 indices!");
					}

					else {
						memcpy(&ourMesh->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
					}

				}
				//For saving all meshes in a list in case we import more than one
				//push meshes
				//meshArray.push_back(ourMesh);
				Buffer(ourMesh);
			}
			else {
				delete ourMesh;
			}
		}
		aiReleaseImport(scene);

	}
	else {
		LOG(LogType::L_ERROR, "Error loading FBX % s", path);

	}
}

void Loader::Buffer(Mesh* Mesh) {
	//Fill buffers with vertices
	glEnableClientState(GL_VERTEX_ARRAY);
	glGenBuffers(1, (GLuint*)&(Mesh->id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, Mesh->id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Mesh->num_vertices * 3, Mesh->vertices, GL_STATIC_DRAW);

	//Fill buffers with indices
	glGenBuffers(1, (GLuint*)&(Mesh->id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * Mesh->num_indices, Mesh->indices, GL_STATIC_DRAW);

	glDisableClientState(GL_VERTEX_ARRAY);


	//Add mesh to meshes vector
	meshArray.push_back(Mesh);
}
// Called before quitting
bool Loader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();
	for (int i = 0; i < meshArray.size(); i++) {
		delete meshArray[i];
		meshArray[i] = nullptr;
	}
	meshArray.clear();
	return true;
}

void Loader::Draw() {
	for(int i = 0; i < meshArray.size(); i++) {
		meshArray[i]->Draw();
	}
}

void Mesh::Draw() {

	//vertices
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	/*glBegin(GL_TRIANGLES);

	for (int i = 0; i < num_indices; i++) {
		glVertex3f(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
	}
	glEnd();*/
}







