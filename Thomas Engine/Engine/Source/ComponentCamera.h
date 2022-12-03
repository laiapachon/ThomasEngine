#pragma once
#include "Component.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera();
	ComponentCamera(GameObject* obj);
	virtual ~ComponentCamera();

	void Update() override;
	void OnEditor() override;

	void PreUpdate();
	void ReGenerateFrameBuffer(int w, int h);
	void PostUpdate();

	bool GetIsMainCamera() { return isMainCamera; };
	void SetIsMainCamera(bool ret) { isMainCamera = ret; };
	bool GetShowFrustrum() { return showFrustrum; };
	void SetShowFrustrum(bool ret) { showFrustrum = ret; };

	void CalculateViewMatrix();
	void ReMakeProjection(float aspectRatio);

	bool updateCamera = true;
	Frustum frustrum;
	float4x4 viewMatrix;

	float FOV = 90.f;
	float aspectRatio = 1;

	// FrameBuffer
	unsigned int framebuffer = 0;
	unsigned int texColorBuffer = 0;
	unsigned int rbo = 0;

private:

	bool isMainCamera = false;
	bool showFrustrum = true;
};