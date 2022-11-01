#include "Transform.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "MathGeoLib/include/Math/TransformOps.h"

Transform::Transform(GameObject* obj) : Component(obj), updateTransform(false)
{
	// Inicialize transforms
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, scale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTransposed = globalTransform.Transposed();
}

Transform::~Transform()
{
}

void Transform::Update()
{
}

void Transform::OnEditor()
{
	// New system more efficient:
	// Instead of creating a 4x4 matrix every time a parameter is modified 
	// which is very expensive to calculate because (float4x4 * float4x4 * float4x4) are a lot of GPU cycles,  
	// We only modify the parameter that has been modified, only position or only rotation or only scale.
	// There is more code but it is more efficient
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Position: ");
		if (ImGui::DragFloat3("##Position", &position[0], 0.1f))
		{
			// Only overwrite position
			localTransform.SetCol3(3, position);
			updateTransform = true;
		}

		ImGui::Text("Rotation: ");
		if (ImGui::DragFloat3("##Rotation", &eulerRotation[0], 0.1f))
		{
			// We need to do this because otherwise in the inspector the rotation "?" and "?" are "-0" instead of "0" 
			if (eulerRotation[0] == 0) eulerRotation[0] = 0;
			if (eulerRotation[2] == 0) eulerRotation[2] = 0;

			// Calculate quaternion
			rotation = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);

			// If the scale has not been modified (sum of the diagonal of matrix = 0) then only overwrite rotate
			// But if the scale yes has been modified then float3x3(rotate) * float3x3::Scale(scale)
			if (localTransform.Trace() == 0)
				localTransform.SetRotatePart(rotation);
			else
				localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));

			updateTransform = true;
		}

		ImGui::Text("Scale: ");
		if (ImGui::DragFloat3("##Scale", &scale[0], 0.1f))
		{
			// If the rotation has not been modified (quaternion = identity) then only overwrite scale
			// But if the rotation yes has been modified then float3x3(rotate) * float3x3::Scale(scale)
			if (rotation.Equals(Quat::identity))
				localTransform.Scale(scale);
			else
				localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));

			updateTransform = true;
		}
		// If some transfomr has been modify update them
		if (updateTransform)
			UpdateTransform();
	}
}

// Update globalTransform of children from the component owner
// Example: if the father moves the children too
void Transform::UpdateTransform()
{
	// Store all children transforms which are pending to update
	std::vector<Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	// transformsToUpdate.size() = number of children from the component owner  
	for (size_t i = 0; i < transformsToUpdate.size(); i++)
	{
		if (transformsToUpdate[i]->GetOwner()->GetParent() != nullptr)
		{
			Transform* parentTra = transformsToUpdate[i]->GetOwner()->GetParent()->transform;

			if (parentTra != nullptr) {
				// global = global parent * local
				transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;
				transformsToUpdate[i]->globalTransformTransposed = transformsToUpdate[i]->globalTransform.Transposed();
			}
		}
	}

	transformsToUpdate.clear();
	updateTransform = false;
}
//Populates an array of childs in descending order
Transform* Transform::GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms)
{
	// Store transforms
	transforms.push_back(node);
	int numChildrens = node->GetOwner()->GetChildren().size();

	// If don't have childrens stop recursive
	if (numChildrens != 0)
	{
		for (int i = 0; i < numChildrens; i++)
		{
			Transform* parentTrans = node->GetOwner()->GetChildren()[i]->transform;
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

	// This is more efficient than localTransform = float4x4::FromTRS()
	localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));
	localTransform.SetCol3(3, position);

	if (parent != nullptr) {
		globalTransform = parent->globalTransform * localTransform;
		globalTransformTransposed = (parent->globalTransform * localTransform).Transposed();
	}
}