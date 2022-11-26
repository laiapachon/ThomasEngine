#pragma once
#include "Component.h"

#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/Quat.h"

class Transform : public Component
{
public:
	Transform(GameObject* obj);
	virtual ~Transform() {};

	void Update() override;
	void OnEditor() override;

	float3 GetPosition() { return position; };

	void SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent);
	float4x4 GetGlobalTransform() {return globalTransform;};
	float* GetGlobalTransformT() {return globalTransformTransposed.ptr();};

	void UpdateTransform();
	void NewAttachment();
	void ResetTransform();

	void UpdateBoundingBoxes();

	float3 GetForward();
	float3 GetUp();
	float3 GetRight();
	float3 GetNormalizeAxis(int i);

private:
	Transform* GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms);

	bool updateTransform = false;

	// Local transforms
	float4x4 localTransform;
	float3 position, scale = { 1,1,1 };
	Quat rotation = Quat::identity; // Quat is best than float3
	float3 eulerRotation = { 0,0,0 }; // Need a container that holds the conversion of Quad to float3 for draw it on OnEditor

	// Globals transforms
	float4x4 globalTransform;
	float4x4 globalTransformTransposed;
};