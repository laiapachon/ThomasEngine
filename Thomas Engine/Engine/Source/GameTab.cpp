#include"Application.h"
#include "GameTab.h"

// Module 
#include "Camera3D.h"

GameTab::GameTab() : Tab()
{
	name = "Game";
}

void GameTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		// Check if some key/mouseButton are pressed
		if (ImGui::IsWindowHovered())
		{
			app->camera->CheckInputs();
		}

		ImVec2 size = ImGui::GetContentRegionAvail();
		app->camera->cameraScene.RecalculateProjection(size.x / size.y);
		ImGui::Image((ImTextureID)app->camera->cameraScene.texColorBuffer, size, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}