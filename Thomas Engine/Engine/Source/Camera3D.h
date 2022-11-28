#pragma once
#include "Module.h"
#include "Math/float3.h"
#include "ComponentCamera.h"
#include "Geometry/LineSegment.h"
#include "Imgui/imgui.h"

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

	void GenerateRay();
	ImVec2 NormalizeOnWindow(float x, float y, float w, float h, ImVec2 point);

	void LookAt(const float3& Spot);
	void Move(const float3& Movement);

	void OrbitRotation();
	void CalculateViewMatrix();

private:
	void Focus();
	void FrontView();

	bool SaveConfig(JsonParser& node)const override;
	bool LoadConfig(JsonParser& node) override;

public:

	float3 right, up, front, position, reference;

	bool projectionIsDirty = false;

	LineSegment ray;
	ComponentCamera cameraScene;

	float cameraSpeed = 10.0f;
	float zoomSpeed = 5.0f;
	float cameraSensitivity = 0.1f;
};