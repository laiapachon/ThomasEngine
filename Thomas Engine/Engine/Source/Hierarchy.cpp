#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "GameObject.h"
#include "Scene.h"
#include "Editor.h"
#include "Inspector.h"
#include "Input.h"

#include <stack>

Hierarchy::Hierarchy(Scene* scene) : Tab(), sceneReference(scene)
{
	name = "Hierarchy";
}

void Hierarchy::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		// If exist some scene and this have root draw all game object tree
		if (sceneReference != nullptr && sceneReference->root != nullptr)
		{
			DrawGameObjectsTree(sceneReference->root, false);
		}
	}
	ImGui::End();
}

void Hierarchy::DrawGameObjectsTree(GameObject* node, bool drawAsDisabled)
{
	std::stack<GameObject*> S;
	std::stack<uint> indents;
	S.push(app->scene->root);
	indents.push(0);

	// No recursive :)
	while (!S.empty())
	{
		GameObject* go = S.top();
		ImGuiTreeNodeFlags nodeFlags = SetFlags(go);
		uint indentsAmount = indents.top();
		S.pop();
		indents.pop();

		for (uint i = 0; i < indentsAmount; ++i)
		{
			ImGui::Indent();
		}

		if (ImGui::TreeNodeEx(go, nodeFlags, go->name.c_str()))
		{
			DragHierarchyObj(go);
			if (ImGui::IsItemClicked() && !go->IsRoot())
			{
				GameObject* &objSelected = static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected;
				objSelected ? objSelected->isSelected = !objSelected->isSelected : 0;
				objSelected = go;
				objSelected->isSelected = !objSelected->isSelected;
			}
			for (GameObject* child : go->GetChildrens())
			{
				S.push(child);
				indents.push(indentsAmount + 1);
			}
			for (uint i = 0; i < indentsAmount; ++i)
			{
				ImGui::Unindent();
			}
			ImGui::TreePop();
		}
	}
}

void Hierarchy::DragHierarchyObj(GameObject*& go)
{
	// Root can't be draged
	if (!go->IsRoot())
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("DragDropHierarchy", &go, sizeof(GameObject*), ImGuiCond_Once);
			ImGui::Text("%s", go->name.c_str());
			ImGui::EndDragDropSource();
		}
	}	

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			GameObject* droppedGo = (GameObject*)*(const int*)payload->Data;
			// droppedGo != go->GetParent() ---> Don't can DragDrop father into his child
			if (droppedGo && droppedGo != go->GetParent())
			{
				bool ret = true;
				GameObject* aux = go;
				GameObject* child = go;
				ret = AreYouMyDescendent(child, droppedGo);
				go = aux;

				if(ret)
				{
					// Erase of child list of his father
					droppedGo->GetParent()->EraseChildren(droppedGo->GetParent()->FindChildren(droppedGo));
					// Attach as new child 
					go->AttachChild(droppedGo);
				}				
			}
		}
		ImGui::EndDragDropTarget();
	}
}

bool Hierarchy::AreYouMyDescendent(GameObject* child, GameObject* droppedGo)
{
	while (child != nullptr)
	{
		if (child->GetParent() == droppedGo) return false;
		child = child->GetParent();
	}
	return true;
}

ImGuiTreeNodeFlags Hierarchy::SetFlags(GameObject* node)
{
	// This flags allow to open the tree if you click on arrow or doubleClick on object, by default the tree is open  
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;

	// If GameObject doesn't childrens = no collapsing and no arrow
	if (node->GetChildrens().size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	// If GameObject is selected = activeModeSelected
	if (node == app->editor->GetGameObjectSelected())
		flags |= ImGuiTreeNodeFlags_Selected;

	return flags;
}
