#pragma once

#include "Mesh.h"
#include "glmath.h"
#include "Glew/include/glew.h"
#include "Module.h"

#include <Assimp/include/cimport.h>
#include <Assimp/include/postprocess.h>
#include <Assimp/include/scene.h>
#include <cassert>
#include "Globals.h"

struct Vertex
{
    vec3 m_pos;
    vec3 m_normal;

    Vertex() {}

    Vertex(const vec3& pos, const vec3& normal)
    {
        m_pos = pos;
        m_normal = normal;
    }

    Vertex(const vec3& pos)
    {
        m_pos = pos;
        m_normal = vec3(0.0f, 0.0f, 0.0f);
    }
};

class MeshData
{
public:

    MeshData();
    ~MeshData();

    bool LoadMesh(const std::string& fileName);
    void Render();

private:

    void InitFromScene(const aiScene* pScene);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(const std::vector<Vertex>& Vertices,
            const std::vector<unsigned int>& Indices);

        GLuint VB;
        GLuint IB;
        unsigned int NumIndices;
    };

    std::vector<MeshEntry> mEntries;
};

class FBXLoader : public Module
{
public:
    FBXLoader(Application* app, bool start_enabled = true);
    ~FBXLoader();

    bool Init();
    update_status PreUpdate(float dt);
    update_status PostUpdate(float dt);

    bool CleanUp();
public:

};
