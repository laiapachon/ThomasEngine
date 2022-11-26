#pragma once
#include "Component.h"

class Texture;
class GameObject;
typedef unsigned int GLuint;

class Material : public Component
{
public:
	Material(GameObject* obj);
	virtual ~Material();

	void OnEditor() override;
	int GetTextureID();

	bool CompareTextureId(GameObject* node, GameObject* owner, GLuint id);

	Texture* texture = nullptr;
private:
	bool viewWithCheckers = false;
};