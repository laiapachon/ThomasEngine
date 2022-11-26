#include "Application.h"

// Module 
#include "Camera3D.h"
#include "Editor.h"
#include "Input.h"

#include "Globals.h" 

#include "GameObject.h"
#include "Inspector.h"

// Components
#include "Transform.h"
#include "MeshRenderer.h"

Camera3D::Camera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera3D";

	ReStartCamera();
}

void Camera3D::ReStartCamera()
{
	right = float3(1.0f, 0.0f, 0.0f);
	up    =	float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(5.0f, 4.0f, 5.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	CalculateViewMatrix();
}

bool Camera3D::Start()
{
	LOG(LogType::L_NORMAL, "Setting up the camera");
	bool ret = true;

	LookAt(float3::zero);

	return ret;
}

bool Camera3D::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Cleaning camera");

	return true;
}

update_status Camera3D::Update(float dt)
{
	return UPDATE_CONTINUE;
}

void Camera3D::OnGUI()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Text("Camera Position:");
		if (ImGui::DragFloat3("##Position", &position[0], 0.1f, true)){}

		ImGui::NewLine();
		ImGui::PushItemWidth(150);

		if (ImGui::SliderFloat("Vert FOV", &verticalFOV, 10,270))
		{
			projectionIsDirty = true;
		}
		if (ImGui::SliderFloat("Near plane", &nearPlaneDistance, 0.1f, 10))
		{
			projectionIsDirty = true;
		}
		if (ImGui::SliderFloat("Far plane", &farPlaneDistance, 11,5000))
		{
			projectionIsDirty = true;
		}
		if (ImGui::SliderFloat("Speed mov", &cameraSpeed, 1, 100))	{}
		if (ImGui::SliderFloat("Sensitivity", &cameraSensitivity, 0.01f, 0.5)){}

		ImGui::PopItemWidth();
		ImGui::NewLine();
	}
}

void Camera3D::CheckInputs()
{
	float3 newPos(0, 0, 0);
	float speed = cameraSpeed * app->GetDt();
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2;

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) Focus();

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += front * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= front * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos += right * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos -= right * speed;

	newPos += front * App->input->GetWheel();

	position += newPos;
	reference += newPos;

	// Mouse motion ----------------
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) OrbitRotation();
}

void Camera3D::OrbitRotation()
{	
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		GameObject* gameObject = App->editor->GetGameObjectSelected();
		float3 posGO = { 0, 0, 0 };

		if (gameObject != nullptr)
			posGO = gameObject->transform->GetPosition();

		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && gameObject != nullptr)
		{
			const float newDeltaX = (float)dx * cameraSensitivity;
			const float newDeltaY = (float)dy * cameraSensitivity;

			reference = posGO;
			Quat pivot = Quat::RotateY(up.y >= 0.f ? newDeltaX * .1f : -newDeltaX * .1f);

			pivot = pivot * math::Quat::RotateAxisAngle(right, newDeltaY * .1f);

			position = pivot * (position - reference) + reference;

			CalculateViewMatrix();
			LookAt(reference);
		}
		else
		{
			if (dx != 0)
			{
				float deltaX = (float)dx * cameraSensitivity;

				Quat rotateY = Quat::RotateY(up.y >= 0.f ? deltaX * .1f : -deltaX * .1f);
				up = rotateY * up;
				front = rotateY * front;
				CalculateViewMatrix();
			}

			if (dy != 0)
			{
				float deltaY = (float)dy * cameraSensitivity;

				Quat rotateX = Quat::RotateAxisAngle(right, -deltaY * .1f);
				up = rotateX * up;
				front = rotateX * front;
				CalculateViewMatrix();
			}
		}		
	}
}

void Camera3D::Focus()
{
	//Focus
	GameObject*& objSelected = static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected;

	if (objSelected != nullptr)
	{
		if (MeshRenderer* mesh = static_cast<MeshRenderer*>(objSelected->GetComponent(ComponentType::MESHRENDERER)))
		{
			const float3 meshCenter = mesh->GetCenterPointInWorldCoords();
			LookAt(meshCenter);
			const float meshRadius = mesh->GetSphereRadius();
			const float currentDistance = meshCenter.Distance(position);
			position = meshCenter + ((position - meshCenter).Normalized() * meshRadius *2);
		}
		else
		{
			float3 pivot = objSelected->transform->GetPosition();
			LookAt(pivot);
		}
	}
}

void Camera3D::FrontView()
{
	GameObject* gameObject = App->editor->GetGameObjectSelected();

	if (gameObject != nullptr)
	{
		float3 posGO = { 0, 0, 0 };
		float3 nwPos;

		posGO = gameObject->transform->GetPosition();

		nwPos = posGO;
		// First param: Right(X), Second param: UP(Y), Third param: From(Z)		
		// With the inverted of the axes the opposite position is obtained
		position = nwPos + float3(0, 0, -10);
		LookAt(nwPos);
	}
}

// -----------------------------------------------------------------
void Camera3D::LookAt( const float3&Spot)
{
	reference = Spot;

	front = (reference - position).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::Move(const float3&Movement)
{
	position += Movement;
	reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::CalculateViewMatrix()
{
	if (projectionIsDirty)
		RecalculateProjection();

	cameraFrustum.pos = position;
	cameraFrustum.front = front.Normalized();
	cameraFrustum.up = up.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	right = up.Cross(front);

	viewMatrix = cameraFrustum.ViewMatrix();
}

void Camera3D::RecalculateProjection()
{
	cameraFrustum.type = FrustumType::PerspectiveFrustum;

	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * PI / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
}

bool Camera3D::SaveConfig(JsonParser& node) const
{
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "X.x", right.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "X.y", right.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "X.z", right.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Y.x", up.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Y.y", up.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Y.z", up.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Z.x", front.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Z.y", front.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Z.z", front.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Reference.x", reference.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Reference.y", reference.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Reference.z", reference.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Position.x", position.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Position.y", position.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Position.z", position.z);

	return true;
}

bool Camera3D::LoadConfig(JsonParser& node)
{
	LOG(LogType::L_NORMAL, "Setting up the camera");

	right.x = (float)node.JsonValToNumber("X.x");
	right.y = (float)node.JsonValToNumber("X.y");
	right.z = (float)node.JsonValToNumber("X.z");
	
	up.x = (float)node.JsonValToNumber("Y.x");
	up.y = (float)node.JsonValToNumber("Y.y");
	up.z = (float)node.JsonValToNumber("Y.z");
	
	front.x = (float)node.JsonValToNumber("Z.x");
	front.y = (float)node.JsonValToNumber("Z.y");
	front.z = (float)node.JsonValToNumber("Z.z");
	
	position.x = (float)node.JsonValToNumber("Position.x");
	position.y = (float)node.JsonValToNumber("Position.y");
	position.z = (float)node.JsonValToNumber("Position.z");

	reference.x = (float)node.JsonValToNumber("Reference.x");
	reference.y = (float)node.JsonValToNumber("Reference.y");
	reference.z = (float)node.JsonValToNumber("Reference.z");

	LookAt(reference);
	RecalculateProjection();

	return true;
}