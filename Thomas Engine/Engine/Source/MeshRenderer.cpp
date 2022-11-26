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

void MeshRenderer::Update()
{
	app->renderer3D->renderQueue.push_back(this);
}

void MeshRenderer::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &active);
		// Print mesh information
		IMGUI_PRINT("Vertices: ", "%i", mesh->numVertex);
		IMGUI_PRINT("Normals: ", "%i", mesh->numNormals);
		IMGUI_PRINT("Tex coords: ", "%i", mesh->numIndexs);
		IMGUI_PRINT("Indices: ", "%i", mesh->numIndexs);

		// You can enable/disable the mode debug
		ImGui::Checkbox("Vertex Normals", &vertexNormals);	ImGui::SameLine();		ImGui::Checkbox("Face Normals", &faceNormals);
		// You can enable/disable the bounding boxes
		ImGui::Checkbox("Show AABB     ", &showAABB);		ImGui::SameLine();		ImGui::Checkbox("Show OBB", &showOBB);
	}
}

void MeshRenderer::RenderMesh()
{
	if (active)
	{
		// Push the Matrix to OpenGL
		glPushMatrix();
		glMultMatrixf(GetOwner()->transform->GetGlobalTransformT());

		// Get material component 
		Material* material = static_cast<Material*>(GetOwner()->GetComponent(ComponentType::MATERIAL));
		GLuint id = -1;
		// If this gameObject has 
		if (material != nullptr)
			id = material->GetTextureID();

		// Draw
		mesh->Render(id);

		// If vertexNormals or faceNormals are true draw the Normals
		if (vertexNormals || faceNormals)
			mesh->DebugRender(&vertexNormals, &faceNormals);


		// Pop the Matrix to OpenGL
		glPopMatrix();
		
		// If showAABB are enable draw the his bounding boxes
		if (showAABB == true) {
			float3 points[8];
			globalAABB.GetCornerPoints(points);
			DrawBoundingBoxes(points, float3(0.2f, 1.f, 0.101f));
		}
		// If showOBB are enable draw the his bounding boxes
		if (showOBB == true) {
			float3 points[8];
			globalOBB.GetCornerPoints(points);
			DrawBoundingBoxes(points);
		}
	}
}

void MeshRenderer::SetBoundingBoxes(Mesh* mesh)
{
	// Generate global AABB
	globalOBB = mesh->localAABB;
	globalOBB.Transform(GetOwner()->transform->GetGlobalTransform());

	// Generate global AABB
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