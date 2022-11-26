#include"Application.h"
#include"Globals.h"
#include "GameTab.h"

// Module 
#include "Renderer3D.h"
#include "Window.h"
#include "Camera3D.h"

Game::Game() : Tab()
{
	name = "Game";
}

void Game::Draw() {

	if (ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

		//checking if any button is pressed while window is hovered
		if (ImGui::IsWindowHovered()) {
			
			app->camera->CheckInputs();

		}

		//create window
		ImVec2 OriginalSize = ImVec2(app->window->GetWindowWidth(), app->window->GetWindowHeight());
		ImVec2 ViewtSize = ImGui::GetWindowSize();

		//recalculate camera projection
		if (ViewtSize.x != lastViewportSize.x || ViewtSize.y != lastViewportSize.y)
		{
			app->camera->aspectRatio = ViewtSize.x / ViewtSize.y;
			app->camera->RecalculateProjection();
		}
		lastViewportSize = ViewtSize;

		//checking if the rendering size is the same
		ImVec2 startPoint = ImVec2((OriginalSize.x / 2) - (ViewtSize.x / 2), (OriginalSize.y / 2) + (ViewtSize.y / 2));
		ImVec2 endPoint = ImVec2((OriginalSize.x / 2) + (ViewtSize.x / 2), (OriginalSize.y / 2) - (ViewtSize.y / 2));


		ImVec2 uv0 = ImVec2(startPoint.x / OriginalSize.x, startPoint.y / OriginalSize.y);
		ImVec2 uv1 = ImVec2(endPoint.x / OriginalSize.x, endPoint.y / OriginalSize.y);



		ImGui::Image((ImTextureID)app->renderer3D->texColorBuffer, ViewtSize, uv0, uv1);
	}
	ImGui::End();

}