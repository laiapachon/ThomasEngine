#pragma once
#include "Component.h"
#include "Geometry/Frustum.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* obj);
	virtual ~ComponentCamera() {};

	void Update() override;
	void OnEditor() override;

	void CalculateViewMatrix();
	void RecalculateProjection();

	bool GetIsMainCamera() { return isMainCamera; };
	void SetIsMainCamera(bool ret) { isMainCamera = ret; };

	bool updateCamera = false;

private:
	Frustum frustrum;

	bool isMainCamera = false;
	bool showFrustrum = false;
	bool projectionIsDirty = false;
};