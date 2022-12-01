#include "Application.h"
#include "Inspector.h"
#include "ResourceManager.h"

#include "GameObject.h"
#include "Component.h"
#include "ResourceTexture.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Guizmo/ImGuizmo.h"
#include "Guizmo/GraphEditor.h"


Inspector::Inspector() : Tab(), gameObjectSelected(nullptr)
{
	name = "Inspector";

	tags = { "Untagged", "Respawn", "Finish", "EditorOnly", "MainCamara", "Player", "GameController" };
	layers = { "Default", "TransparentFX", "Ignore Raycast", "Water", "UI", "Player" };
}

void Inspector::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		// The inspector is empty if no object is selected 
		if (gameObjectSelected != nullptr)
		{
			if (item == ItemType::NONE) {

				DrawDefaultInspector();
			}
			else {

				DrawEditLists();		
				// TODO guizmo 
				/*
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
				*/
			}
		}		
	}
	ImGui::End();
}

void Inspector::DrawDefaultInspector()
{
	// If you write "##" the name become on label and this no draw
	if (ImGui::Checkbox("##Active", &gameObjectSelected->active))
	{
		//Warning: The bool has changed at this point, you have to do the reverse
		(gameObjectSelected->active == true) ? gameObjectSelected->Enable() : gameObjectSelected->Disable();
	}
	ImGui::SameLine();

	// Print the name of GameObject and the static checkbox
	char* inputName = &gameObjectSelected->name[0];
	ImGui::InputText("##Name", inputName, gameObjectSelected->name.size() + 1);
	ImGui::SameLine();
	ImGui::Checkbox("Static", &gameObjectSelected->isStatic);

	// Draw tagList and layerList
	DrawList("Tag", &tags, gameObjectSelected->tag, maxWidthTag);
	ImGui::SameLine();
	DrawList("Layer", &layers, gameObjectSelected->layer, maxWidthLayers);

	// Destroy object selected, pendingToDelete = true
	if (ImGui::Button("Delete")) {
		gameObjectSelected->Destroy();
	}
	ImGui::Separator();

	// Draw all OnEditors componets
	for (size_t i = 0; i < gameObjectSelected->GetCompomemts().size(); i++)
	{
		gameObjectSelected->GetCompomemts()[i]->OnEditor();
		ImGui::Separator();
	}

}

void Inspector::DrawEditLists()
{
	if (ImGui::ImageButton((ImTextureID)app->resourceManager->backButton->textureID, ImVec2(16, 16)))
		item = ItemType::NONE;

	// System to determine which node starts open 
	ImGuiTreeNodeFlags flag = 0;
	if (item == ItemType::TAG) flag = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::CollapsingHeader("Tags", flag))
	{
		DrawListTagLayer("Tag", tags);
	}

	if (item == ItemType::LAYER) flag = ImGuiTreeNodeFlags_DefaultOpen;
	else flag = 0;
	if (ImGui::CollapsingHeader("Layers", flag))
	{
		DrawListTagLayer("Layer", layers);
	}
}

void Inspector::DrawListTagLayer(const char* label, std::vector<std::string> list)
{
	for (int i = 0; i < list.size(); i++)
	{
		ImGui::Text("%s %d: ",label, i);	ImGui::SameLine();
		ImGui::Text(list.at(i).c_str());
	}
	ImGui::Separator();
	ImGui::Text("New %s: ", label);	ImGui::SameLine();

	ImGui::PushItemWidth(100);
	if (label == "Tag")
		ImGui::InputTextWithHint("  ", "Add new Tag", newTag, IM_ARRAYSIZE(newTag));
	else
		ImGui::InputTextWithHint(" ","Add new Layer", newLayer, IM_ARRAYSIZE(newLayer));
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushID(label);
	if (ImGui::ImageButton((ImTextureID)app->resourceManager->addButton->textureID, ImVec2(12, 12)))
	{
		if (label == "Tag")
		{
			AddTag(newTag);
			newTag[0] = NULL;
		}
		else if (newLayer)
		{
			AddLayer(newLayer);
			newLayer[0] = NULL;
		}
	}
	ImGui::PopID();
	
}

std::string Inspector::DrawList(const char* label, std::vector<std::string>* list, std::string &item, int width)
{
	std::string ret = "";
	ImGui::Text(label); ImGui::SameLine();

	// Set the width of item
	ImGui::PushItemWidth(width);

	std::string listLabel = "##List";
	listLabel.append(label);

	// Draw the popUp tab of a list and the state of current item of in the list has a special style
	if (ImGui::BeginCombo(listLabel.c_str(), item.c_str()))
	{
		for (int i = 0; i < list->size(); i++)
		{
			bool isSelected = !item.compare(list->at(i));
			if (ImGui::Selectable(list->at(i).c_str(), isSelected))
				item = list->at(i);
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::Separator();

		AddItem(label);

		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	return ret;
}

void Inspector::AddItem(const char* label)
{
	std::string addItem = "Add";
	addItem.append(label);
	if (ImGui::Button(addItem.c_str()))
	{
		if (label == "Tag")
			item = ItemType::TAG;
		else if (label == "Layer")
			item = ItemType::LAYER;
	}
}

void Inspector::AddTag(std::string newTag)
{
	if (newTag != "")
	{
		tags.push_back(newTag);
		CalculateMaxWidth(tags, maxWidthTag);
	}	
}

void Inspector::AddLayer(std::string newLayer)
{
	if (newLayer != "")
	{
		layers.push_back(newLayer);
		CalculateMaxWidth(layers, maxWidthLayers);
	}
}

// This function calculate the size of the longest text in a string list in pixels
void Inspector::CalculateMaxWidth(std::vector<std::string> list, int& width)
{
	int w = width;
	for (int i = 0; i < list.size(); i++)
	{
		if (ImGui::CalcTextSize(list.at(i).c_str()).x > width)
			width = ImGui::CalcTextSize(list[i].c_str()).x;
	}

	// Add a margin: GetFrameHeight = button size
	// ItemInnerSpacing.x is the space from the beginning of the container until the first character is drawn 
	// Only if the size has increased
	if (w != width)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		width += ImGui::GetFrameHeight() + style.ItemInnerSpacing.x * 2;
	}	
}
