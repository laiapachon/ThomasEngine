#pragma once
#include "Module.h"

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

class Camera3D : public Module
{
public:
	Camera3D(Application* app, bool start_enabled = true);
	~Camera3D() {};

	void ReStartCamera();
	bool Start() override;

	update_status Update(float dt) override;
	void OnGUI() override;

	void CheckInputs();
	bool CleanUp() override;

	void LookAt(const float3&Spot);
	void Move(const float3&Movement);

	float4x4 GetViewMatrix() { return viewMatrix; };

	void OrbitRotation();
	void CalculateViewMatrix();
	void RecalculateProjection();

private:
	void Focus();
	void FrontView();

	bool SaveConfig(JsonParser& node)const override;
	bool LoadConfig(JsonParser& node) override;

public:
	
	float3 right, up, front, position, reference;

	bool projectionIsDirty = false;

	Frustum cameraFrustum;
	float4x4 viewMatrix;

	float cameraSpeed = 5.0f;
	float cameraSensitivity = 0.15f;

	float aspectRatio = 1.f;
	float verticalFOV = 90.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 5000.f;
};