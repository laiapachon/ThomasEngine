#pragma once
#include "Component.h"

#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/Quat.h"

class Transform : public Component
{
public:
	Transform(GameObject* obj);
	virtual ~Transform();

	void Update() override;
	void OnEditor() override;

	float3 GetPosition() { return position; };

	void SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent);
	float* GetGlobalTransformT() { return globalTransformTransposed.ptr(); };

private:
	void UpdateTransform();
	Transform* GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms);

	bool updateTransform = true;

	// Local transforms
	float4x4 localTransform;
	float3 position, scale = { 0,0,0 };
	Quat rotation = Quat::identity; // Quat is better than float3
	float3 eulerRotation = { 0,0,0 }; // Need a container that holds the conversion of Quad to float3 for draw it on OnEditor

	// Globals transforms
	float4x4 globalTransform;
	float4x4 globalTransformTransposed;
};