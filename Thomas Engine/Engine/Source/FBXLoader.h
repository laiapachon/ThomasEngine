#pragma once

#include "Module.h"
#include "Globals.h"
#include <vector>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

using namespace std;

struct Mesh {
	uint id_indices = 0; // index in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;
	uint id_vertices = 0; // unique vertex in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	void Draw();
};

class Loader : public Module
{
public:
	Loader(Application* app, bool start_enabled = true);
	~Loader();
	
	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	void LoadFile(string path, vector <Mesh*> meshArray );
	void Draw();


	bool CleanUp();



public:

	vector <Mesh*> meshArray;
	string path;

private:
	
};