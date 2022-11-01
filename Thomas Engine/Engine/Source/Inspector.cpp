#include "Application.h"
#include "Inspector.h"

#include "GameObject.h"
#include "Component.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

Inspector::Inspector() : Tab(), gameObjectSelected(nullptr)
{
	name = "Inspector";

	tags = { "Untagged", "Respawn", "Finish", "EditorOnly", "MainCamara", "Player", "GameController" };
	layers = { "0: Default", "1: TransparentFX", "2: Ignore Raycast", "3: Water", "4: UI", "5: Player" };
}

Inspector::~Inspector()
{
}

void Inspector::Draw()
{
	if (ImGui::Begin(name.c_str()))
	{
		if (gameObjectSelected != nullptr && !gameObjectSelected->IsRoot())
		{
			// Create the active checkBox
			if (ImGui::Checkbox("##Active", &gameObjectSelected->active))
			{
				//The bool has changed on the checkbox call at this point
				(gameObjectSelected->active == true) ? gameObjectSelected->Enable() : gameObjectSelected->Disable();
			}

			ImGui::SameLine();
			// Print the name of GameObject and the static checkbox
			char* inputName = &gameObjectSelected->name[0];
			ImGui::InputText("##Name", inputName, gameObjectSelected->name.size() + 1, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
			ImGui::Checkbox("Static", &gameObjectSelected->isStatic);

			// Change to dynamic array by can add new tags
			ImGui::Text("Tag"); ImGui::SameLine();
			static const char* current_item = NULL;

			// Draw the popUp tab of a list and selectedable 
			ImGuiStyle& style = ImGui::GetStyle();
			float w = ImGui::CalcItemWidth();
			float spacing = style.ItemInnerSpacing.x;
			float button_sz = ImGui::GetFrameHeight();
			ImGui::PushItemWidth((w - spacing * 2.0f - button_sz * 2.0f) * 0.5f);

			if (ImGui::BeginCombo("##custom combo", current_item))
			{
				for (int n = 0; n < tags.size(); n++)
				{
					bool is_selected = (current_item == tags.at(n).c_str());
					if (ImGui::Selectable(tags.at(n).c_str(), is_selected))
						current_item = tags.at(n).c_str();
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();
			// Change to dynamic array by can add new layers
			ImGui::Text("Layer"); ImGui::SameLine();

			style = ImGui::GetStyle();
			w = ImGui::CalcItemWidth();
			spacing = style.ItemInnerSpacing.x;
			button_sz = ImGui::GetFrameHeight();
			ImGui::PushItemWidth((w - spacing * 2.0f - button_sz * 2.0f) * 0.5f);

			if (ImGui::BeginCombo("##custom combo 2", current_item))
			{
				for (int n = 0; n < layers.size(); n++)
				{
					bool is_selected = (current_item == layers.at(n).c_str());
					if (ImGui::Selectable(layers.at(n).c_str(), is_selected))
						current_item = layers.at(n).c_str();
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			// Destroy object selected
			if (ImGui::Button("Delete")) {
				gameObjectSelected->Destroy();
			}

			ImGui::Separator();
			// Draw all OnEditors componets
			for (size_t i = 0; i < gameObjectSelected->GetComponents().size(); i++)
			{
				gameObjectSelected->GetComponents()[i]->OnEditor();
			}
		}
	}

	ImGui::End();
}