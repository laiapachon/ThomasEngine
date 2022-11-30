#include "Application.h"
#include "MeshRenderer.h"

#include "Renderer3D.h"
#include "Editor.h"

#include "GameObject.h"
#include "Transform.h"
#include "Material.h"

#include "ImGui/imgui.h"

MeshRenderer::MeshRenderer(GameObject* obj) : Component(obj)
{
}

MeshRenderer::~MeshRenderer()
{
	delete mesh; mesh = nullptr;
}

void MeshRenderer::Update()
{
	app->renderer3D->renderQueue.push_back(this);
}

void MeshRenderer::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &active);
		IMGUI_PRINT("Vertices: ", "%i", mesh->numVertex);
		IMGUI_PRINT("Normals: ", "%i", mesh->numNormals);
		IMGUI_PRINT("Tex coords: ", "%i", mesh->numIndexs);
		IMGUI_PRINT("Indices: ", "%i", mesh->numIndexs);

		ImGui::Checkbox("Vertex Normals", &vertexNormals);	ImGui::SameLine();		
		ImGui::Checkbox("Face Normals", &faceNormals);
		ImGui::Checkbox("Show AABB     ", &showAABB);		ImGui::SameLine();		
		ImGui::Checkbox("Show OBB", &showOBB);
	}
}

void MeshRenderer::RenderMesh()
{
	if (active)
	{
		glPushMatrix();
		glMultMatrixf(GetOwner()->transform->GetGlobalTransformT().ptr());

		Material* material = static_cast<Material*>(GetOwner()->GetComponent(ComponentType::MATERIAL));
		GLuint id = -1;
		if (material != nullptr)
			id = material->GetTextureID();

		mesh->Render(id);

		if (vertexNormals || faceNormals)
			mesh->DebugRender(&vertexNormals, &faceNormals);

		glPopMatrix();
		
		if (showAABB == true) {
			float3 points[8];
			globalAABB.GetCornerPoints(points);
			DrawBoundingBoxes(points, float3(0.2f, 1.f, 0.101f));
		}
		if (showOBB == true) {
			float3 points[8];
			globalOBB.GetCornerPoints(points);
			DrawBoundingBoxes(points);
		}
	}
}

void MeshRenderer::SetBoundingBoxes(Mesh* mesh)
{
	globalOBB = mesh->localAABB;
	globalOBB.Transform(GetOwner()->transform->GetGlobalTransform());

	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(globalOBB);
}

float3 MeshRenderer::GetCenterPointInWorldCoords()
{
	return GetOwner()->transform->GetGlobalTransform().TransformPos(mesh->centerPoint);
}

float MeshRenderer::GetSphereRadius()
{
	return mesh->radius;
}