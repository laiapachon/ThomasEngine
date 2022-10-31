#pragma once
#include "Tab.h"

class Scene;
class GameObject;

class Hierarchy : public Tab
{
public:
	Hierarchy(Scene* scene);
	virtual ~Hierarchy();

	void Draw() override;
	void SetCurrentScene(Scene* scene);

private:
	void DrawGameObjectsTree(GameObject* node, bool drawAsDisabled);

	Scene* sceneReference;
};