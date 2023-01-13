#pragma once
#include "Component.h"
#include "Shader.h"

class Texture;
class GameObject;
class ShaderEditor;
typedef unsigned int GLuint;

class Material : public Component
{
public:
	Material(GameObject* obj);
	virtual ~Material();

	void OnEditor() override;
	int GetTextureID();

	void Bind();

	bool CompareTextureId(GameObject* node, GameObject* owner, GLuint id);

	Texture* texture = nullptr;

	Shader* shader = nullptr;
private:
	bool viewWithCheckers = false;

	ShaderEditor* textEditor;

};