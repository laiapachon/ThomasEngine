#include "Application.h"
#include "ComponentCamera.h"

#include "Globals.h"
#include "Scene.h"
#include "Camera3D.h"
#include "Window.h"

#include "Transform.h"

#include "ImGui/imgui.h"


ComponentCamera::ComponentCamera() : Component(nullptr)
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.nearPlaneDistance = 0.1f;
	frustrum.farPlaneDistance = 500.f;
	frustrum.front = float3::unitZ;
	frustrum.up = float3::unitY;

	frustrum.verticalFov = 60.0f * DEGTORAD;
	frustrum.horizontalFov = 2.0f * atanf(tanf(frustrum.verticalFov / 2.0f) * (16.f / 9.f));

	frustrum.pos = float3::zero;
}

ComponentCamera::ComponentCamera(GameObject* obj) : Component(obj)
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.nearPlaneDistance = 1;
	frustrum.farPlaneDistance = 100.f;
	frustrum.front = obj->transform->GetForward();
	frustrum.up = obj->transform->GetUp();

	frustrum.verticalFov = 60.0f * DEGTORAD;
	frustrum.horizontalFov = 2.0f * atanf(tanf(frustrum.verticalFov / 2.0f) * (16.f / 9.f));

	frustrum.pos = obj->transform->GetPosition();
}

ComponentCamera::~ComponentCamera()
{
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteRenderbuffers(1, &rbo);
}

void ComponentCamera::Update()
{
	if (updateCamera)
	{
		frustrum.pos = GetOwner()->transform->GetPosition();
		frustrum.front = GetOwner()->transform->GetForward();
		frustrum.up = GetOwner()->transform->GetUp();
		CalculateViewMatrix();
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
			{
				app->scene->mainCamera = this;
				ReGenerateFrameBuffer(app->window->GetWindowWidth(), app->window->GetWindowHeight());
			}
			else
				app->scene->mainCamera = nullptr;
		}
		ImGui::Checkbox("Show Frustrum Boxes", &showFrustrum);

		ImGui::PushItemWidth(150);

		ImGui::DragFloat("Verti FOV: ", &frustrum.verticalFov, 0.1f, 0.0f, 300.f);
		ImGui::DragFloat("Horiz FOV: ", &frustrum.horizontalFov, 0.1f, 0.0f, 300.f);

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

void ComponentCamera::PreUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(frustrum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix.Transposed().ptr());
}

void ComponentCamera::ReGenerateFrameBuffer(int w, int h)
{
	ReMakeProjection(w / h);

	if (framebuffer > 0)
		glDeleteFramebuffers(1, &framebuffer);

	if (texColorBuffer > 0)
		glDeleteTextures(1, &texColorBuffer);

	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);


	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG(LogType::L_ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentCamera::PostUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentCamera::CalculateViewMatrix()
{
	updateCamera = false;

	frustrum.pos = GetOwner()->transform->GetPosition();
	frustrum.front = GetOwner()->transform->GetForward();
	frustrum.up = GetOwner()->transform->GetUp();

	viewMatrix = frustrum.ViewMatrix();
}

void ComponentCamera::ReMakeProjection(float aspectR)
{
	aspectRatio = aspectR;
	frustrum.type = FrustumType::PerspectiveFrustum;

	frustrum.verticalFov = (FOV * PI / 2) / 180.f;
	frustrum.horizontalFov = 2.f * atanf(tanf(frustrum.verticalFov * 0.5f) * aspectRatio);
}