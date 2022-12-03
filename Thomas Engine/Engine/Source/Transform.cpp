#include "Application.h"
#include "Scene.h"
#include "Globals.h"
#include "ResourceMesh.h"
#include "Camera3D.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentCamera.h"

#include "ImGui/imgui.h"
#include "MathGeoLib/include/Math/TransformOps.h"
#include "Guizmo/ImGuizmo.h"

Transform::Transform(GameObject* obj) : Component(obj)
{

	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, scale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTransposed = globalTransform.Transposed();
}

void Transform::Update()
{
}

void Transform::OnEditor()
{
	
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Position: ");
		if (ImGui::DragFloat3("##Position", &position[0], 0.1f, true))
		{
			localTransform.SetCol3(3, position);
			updateTransform = true;
		}

		ImGui::Text("Rotation: ");
		if (ImGui::DragFloat3("##Rotation", &eulerRotation[0], 0.1f, true))
		{
			if (eulerRotation[0] == 0) eulerRotation[0] = 0;
			if (eulerRotation[2] == 0) eulerRotation[2] = 0;

			rotation = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);

			
			if (localTransform.Trace() == 0)
				localTransform.SetRotatePart(rotation);
			else
				localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));

			updateTransform = true;
		}

		ImGui::Text("Scale: ");
		if (ImGui::DragFloat3("##Scale", &scale[0], 0.1f, true))
		{
			if (rotation.Equals(Quat::identity))
			{
				localTransform[0][0] = scale.x;
				localTransform[1][1] = scale.y;
				localTransform[2][2] = scale.z;
			}
			else localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));
			
			updateTransform = true;
		}
		CheckStateMode();

		ImGui::NewLine();
		
		if (ImGui::Button("Reset Transform"))
			ResetTransform();

		
		if (updateTransform)
			UpdateTransform();
	}
}


void Transform::UpdateTransform()
{
	
	std::vector<Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	
	for (size_t i = 0; i < transformsToUpdate.size(); i++)
	{
		if (transformsToUpdate[i]->GetOwner()->GetParent() != nullptr)
		{
			Transform* parentTra = transformsToUpdate[i]->GetOwner()->GetParent()->transform;

			if (parentTra != nullptr) {
				
				transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;
				transformsToUpdate[i]->globalTransformTransposed = transformsToUpdate[i]->globalTransform.Transposed();

				
				transformsToUpdate[i]->UpdateBoundingBoxes();
				ComponentCamera* camera = static_cast<ComponentCamera*>(GetOwner()->GetComponent(ComponentType::CAMERA));
				if (camera != nullptr) camera->updateCamera = true;
			}
		}
	}

	transformsToUpdate.clear();
	updateTransform = false;
}


Transform* Transform::GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms)
{
	
	transforms.push_back(node);
	int numChildrens = node->GetOwner()->GetChildrens().size();

	
	if (numChildrens != 0)
	{
		for (int i = 0; i < numChildrens; i++)
		{
			Transform* parentTrans = node->GetOwner()->GetChildrens()[i]->transform;
			GetRecursiveTransforms(parentTrans, transforms);
		}
	}

	return nullptr;
}

void Transform::SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = localScale;

	eulerRotation = this->rotation.ToEulerXYZ() * RADTODEG;

	
	localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));
	localTransform.SetCol3(3, position);

	if (parent != nullptr) {
		globalTransform = parent->globalTransform * localTransform;
		globalTransformTransposed = (parent->globalTransform * localTransform).Transposed();
	}
}

void Transform::NewAttachment()
{
	if (GetOwner()->GetParent() != app->scene->root)
		localTransform = GetOwner()->GetParent()->transform->globalTransform.Inverted().Mul(globalTransform);

	localTransform.Decompose(position, rotation, scale);
	eulerRotation = rotation.ToEulerXYZ();
}

void Transform::ResetTransform()
{
	
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, scale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTransposed = globalTransform.Transposed();

	updateTransform = true;
}

void Transform::UpdateBoundingBoxes()
{
	MeshRenderer* mesh = nullptr;
	mesh = static_cast<MeshRenderer*>(GetOwner()->GetComponent(ComponentType::MESH_RENDERER));
	if (mesh != nullptr)
	{
		mesh->globalOBB = mesh->GetMesh()->localAABB;
		mesh->globalOBB.Transform(globalTransform);

		mesh->globalAABB.SetNegativeInfinity();
		mesh->globalAABB.Enclose(mesh->globalOBB);
	}	
}

float3 Transform::GetRight()
{
	return GetNormalizeAxis(0);
}
float3 Transform::GetUp()
{
	return GetNormalizeAxis(1);
}
float3 Transform::GetForward()
{
	return GetNormalizeAxis(2);
}

float3 Transform::GetNormalizeAxis(int i)
{
	return globalTransform.RotatePart().Col(i).Normalized();
}

void Transform::CheckStateOperation()
{
	if (ImGui::IsKeyPressed(26))
		app->camera->operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(8))
		app->camera->operation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(21))
		app->camera->operation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", app->camera->operation == ImGuizmo::TRANSLATE))
		app->camera->operation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", app->camera->operation == ImGuizmo::ROTATE))
		app->camera->operation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", app->camera->operation == ImGuizmo::SCALE))
		app->camera->operation = ImGuizmo::SCALE;
}

void Transform::CheckStateMode()
{
	if (app->camera->operation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", app->camera->mode == ImGuizmo::LOCAL))
			app->camera->mode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", app->camera->mode == ImGuizmo::WORLD))
			app->camera->mode = ImGuizmo::WORLD;
	}
}

void Transform::SetTransformMFromM(float4x4 globalMatrix)
{
	globalTransform = globalMatrix;
	localTransform = GetOwner()->GetParent()->transform->globalTransform.Inverted() * globalTransform;

	localTransform.Decompose(position, rotation, scale);
	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	globalTransformTransposed = globalTransform.Transposed();

	updateTransform = true;
}

void Transform::operator=(Transform* transform)
{
	this->eulerRotation = transform->eulerRotation;
	this->globalTransform = transform->globalTransform;
	this->globalTransformTransposed = transform->globalTransformTransposed;
	this->localTransform = transform->localTransform;
	this->position = transform->position;
	this->rotation = transform->rotation;
	this->scale = transform->scale;
	this->worldEulerRotation = transform->worldEulerRotation;
	this->worldPosition = transform->worldPosition;
	this->worldRotation = transform->worldRotation;
	this->worldScale = transform->worldScale;
}