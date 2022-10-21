
#include "Application.h"
#include "FBXLoader.h"
#include "Renderer3D.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "GPUDetected/DeviceId.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Assimp/libx86/assimp.lib")

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
	LoadFile(path, meshes);

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
void LoadFile(string path, vector <Mesh*> meshArray) {

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene -> HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++) {
			Mesh* ourMesh = new Mesh();
			// copy vertices
			ourMesh->num_vertices = scene->mMeshes[i]->mNumVertices;
			ourMesh->vertices = new float[ourMesh->num_vertices * 3];
			memcpy(ourMesh->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * ourMesh->num_vertices * 3);
			LOG(LogType::L_ERROR, "New mesh with % d vertices", ourMesh->num_vertices);

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
				//para gaurdar todas las meshes en una lista, por si importas mas de una cosa
				//push meshes
				meshArray.push_back(ourMesh);
			}
			else {
				delete ourMesh;
			}
		}
		aiReleaseImport(scene);
	}
	else {
		LOG(LogType::L_ERROR, "Error loading scene % s", path);

	}
		
}
// Called before quitting
bool Loader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void Loader::Draw() {
	for(int i = 0; i < meshes.size(); i++) {
		meshes[i]->Draw();
	}
}

void Mesh::Draw() {

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < num_indices; i++) {
		glVertex3f(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
	}
	glEnd();
}







