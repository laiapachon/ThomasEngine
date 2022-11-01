#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "GameObject.h"
#include "Scene.h"
#include "Editor.h"
#include "Inspector.h"
#include "Input.h"

Hierarchy::Hierarchy(Scene* scene) : Tab(), sceneReference(scene)
{
	name = "Hierarchy";
}

Hierarchy::~Hierarchy()
{
	sceneReference = nullptr;
}

void Hierarchy::Draw()
{
	if (ImGui::Begin(name.c_str()))
	{
		// If exist some scene and this have root draw all game object tree
		if (sceneReference != nullptr && sceneReference->root != nullptr)
		{
			DrawGameObjectsTree(sceneReference->root, false);
		}
	}
	ImGui::End();
}

void Hierarchy::SetCurrentScene(Scene* scene)
{
	sceneReference = scene;
}

void Hierarchy::DrawGameObjectsTree(GameObject* node, bool drawAsDisabled)
{
	// If this game object is active false draw on mode disabled
	if (drawAsDisabled == false)
		drawAsDisabled = !node->active;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (node->GetChildren().size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (node == app->editor->GetGameObjectSelected())
		flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

	if (drawAsDisabled)
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

	bool nodeOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());

	if (drawAsDisabled)
		ImGui::PopStyleColor();



	if (ImGui::IsItemClicked() && !node->IsRoot())
	{
		dynamic_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected = node;
	}

	node->SetShowChildrens((node->GetChildren().size() == 0) ? false : nodeOpen);

	if (node->GetShowChildrens() == true)
	{
		// Call function recursive mode
		for (unsigned int i = 0; i < node->GetChildren().size(); i++)
		{
			DrawGameObjectsTree(node->GetChildren()[i], drawAsDisabled);
		}
		ImGui::TreePop();
	}
}