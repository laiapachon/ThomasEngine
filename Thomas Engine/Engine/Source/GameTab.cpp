#include"Application.h"
#include "GameTab.h"
#include "Scene.h"


#include "Camera3D.h"

GameTab::GameTab() : Tab()
{
	name = "Game";
}

void GameTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		if (app->scene->mainCamera != nullptr)
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			app->scene->mainCamera->ReMakeProjection(size.x / size.y);
			ImGui::Image((ImTextureID)app->scene->mainCamera->texColorBuffer, size, ImVec2(0, 1), ImVec2(1, 0));
		}
	}
	ImGui::End();
}