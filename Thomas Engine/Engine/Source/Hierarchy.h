#pragma once
#include "Tab.h"
#include "imgui/imgui.h"

class Scene;
class GameObject;

class Hierarchy : public Tab
{
public:
	Hierarchy(Scene* scene);
	virtual ~Hierarchy() { sceneReference = nullptr; };

	void Draw() override;
	void SetCurrentScene(Scene* scene) { sceneReference = scene; };

private:
	void DrawGameObjectsTree(GameObject* node, bool drawAsDisabled);
	void DragHierarchyObj(GameObject*& go);

	bool AreYouMyDescendent(GameObject* child, GameObject* droppedGo);

	ImGuiTreeNodeFlags SetFlags(GameObject* node);

	Scene* sceneReference;
};
