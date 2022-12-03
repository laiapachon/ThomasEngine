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
	
		if (gameObjectSelected != nullptr)
		{
			if (item == ItemType::NONE) {

				DrawDefaultInspector();
			}
			else {

				DrawEditLists();		
				
			}
		}		
	}
	ImGui::End();
}

void Inspector::DrawDefaultInspector()
{
	
	if (ImGui::Checkbox("##Active", &gameObjectSelected->active))
	{
	
		(gameObjectSelected->active == true) ? gameObjectSelected->Enable() : gameObjectSelected->Disable();
	}
	ImGui::SameLine();

	char* inputName = &gameObjectSelected->name[0];
	ImGui::InputText("##Name", inputName, gameObjectSelected->name.size() + 1);
	ImGui::SameLine();
	ImGui::Checkbox("Static", &gameObjectSelected->isStatic);

	DrawList("Tag", &tags, gameObjectSelected->tag, maxWidthTag);
	ImGui::SameLine();
	DrawList("Layer", &layers, gameObjectSelected->layer, maxWidthLayers);

	if (ImGui::Button("Delete")) {
		gameObjectSelected->Destroy();
	}
	ImGui::Separator();

	for (size_t i = 0; i < gameObjectSelected->GetComponents().size(); i++)
	{
		gameObjectSelected->GetComponents()[i]->OnEditor();
		ImGui::Separator();
	}

}

void Inspector::DrawEditLists()
{
	if (ImGui::ImageButton((ImTextureID)app->resourceManager->backButton->textureID, ImVec2(16, 16)))
		item = ItemType::NONE;

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

	ImGui::PushItemWidth(width);

	std::string listLabel = "##List";
	listLabel.append(label);

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


void Inspector::CalculateMaxWidth(std::vector<std::string> list, int& width)
{
	int w = width;
	for (int i = 0; i < list.size(); i++)
	{
		if (ImGui::CalcTextSize(list.at(i).c_str()).x > width)
			width = ImGui::CalcTextSize(list[i].c_str()).x;
	}
	if (w != width)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		width += ImGui::GetFrameHeight() + style.ItemInnerSpacing.x * 2;
	}	
}
