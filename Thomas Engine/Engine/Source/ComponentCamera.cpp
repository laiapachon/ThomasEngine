#include "Application.h"
#include "ComponentCamera.h"
#include "Transform.h"
#include "Globals.h"
#include "Scene.h"

#include "ImGui/imgui.h"

ComponentCamera::ComponentCamera(GameObject* obj) : Component(obj)
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.nearPlaneDistance = 1;
	frustrum.farPlaneDistance = 100.f;
	frustrum.front = obj->transform->GetForward();
	frustrum.up = obj->transform->GetUp();

	frustrum.verticalFov = 60.0f * DEGTORAD;
	frustrum.horizontalFov = 2.0f * atanf(tanf(frustrum.verticalFov / 2.0f) * 1.3f);

	frustrum.pos = obj->transform->GetPosition();
}

void ComponentCamera::Update()
{
	if (updateCamera)
	{
		frustrum.pos = GetOwner()->transform->GetPosition();
		frustrum.front = GetOwner()->transform->GetForward();
		frustrum.up = GetOwner()->transform->GetUp();
	}	

	if (showFrustrum == true) {
		float3 points[8];
		frustrum.GetCornerPoints(points);
		DrawBoundingBoxes(points, float3(0.2f, 1.f, 0.101f));
	}
}

void ComponentCamera::OnEditor()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Main Camera", &isMainCamera))
		{
			if (app->scene->mainCamera != nullptr)
				app->scene->mainCamera->isMainCamera = false;
			if (isMainCamera)
				app->scene->mainCamera = this;
			else
				app->scene->mainCamera = nullptr;
		}
		ImGui::Checkbox("Show Frustrum Boxes", &showFrustrum);

		ImGui::PushItemWidth(150);

		if (ImGui::SliderFloat("Vert FOV", &frustrum.verticalFov, 10, 270))
		{
			projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Near plane", &frustrum.nearPlaneDistance, 0.1f))
		{
			if (frustrum.nearPlaneDistance < 0.1f) frustrum.nearPlaneDistance = 0.1f;
			if (frustrum.nearPlaneDistance > frustrum.farPlaneDistance) frustrum.nearPlaneDistance = frustrum.farPlaneDistance - 0.1f;
		}
		if (ImGui::DragFloat("Far plane", &frustrum.farPlaneDistance, 0.1f))
		{
			if (frustrum.farPlaneDistance < frustrum.nearPlaneDistance) frustrum.farPlaneDistance = frustrum.nearPlaneDistance + 0.1f;
		}
		ImGui::PopItemWidth();
	}
}

void ComponentCamera::CalculateViewMatrix()
{
	/*if (projectionIsDirty)
		RecalculateProjection();

	frustrum.pos = GetOwner()->transform->GetPosition();
	frustrum.front = frustrum.front.Normalized();
	frustrum.up = frustrum.up.Normalized();
	float3::Orthonormalize(frustrum.front, frustrum.up);

	viewMatrix = cameraFrustum.ViewMatrix();*/
}

void ComponentCamera::RecalculateProjection()
{
	/*frustrum.type = FrustumType::PerspectiveFrustum;

	frustrum.nearPlaneDistance = nearPlaneDistance;
	frustrum.farPlaneDistance = farPlaneDistance;
	frustrum.verticalFov = (verticalFOV * PI / 2) / 180.f;
	frustrum.horizontalFov = 2.f * atanf(tanf(frustrum.verticalFov * 0.5f) * aspectRatio);*/
}
