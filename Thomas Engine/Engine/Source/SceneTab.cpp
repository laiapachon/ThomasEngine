#include"Application.h"
#include "SceneTab.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Camera3D.h"

//#include "OpenGL.h"

SceneTab::SceneTab() : Tab()
{
	name = "Scene";
}

SceneTab::~SceneTab()
{

}

void SceneTab::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL , ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		// Check if some key/mouseButton are pressed
		if (ImGui::IsWindowHovered())
		{
			app->camera->CheckInputs();
		}
		// Calculate size of tab scene, get window width and hight and transform the viewport to image and render it 
		ImVec2 texOriginalSize = ImVec2(app->window->GetWindowWidth(), app->window->GetWindowHeight());
		ImVec2 e = ImGui::GetWindowSize();

		ImVec2 startPoint = ImVec2((texOriginalSize.x / 2) - (e.x / 2), (texOriginalSize.y / 2) + (e.y / 2));
		ImVec2 endPoint = ImVec2((texOriginalSize.x / 2) + (e.x / 2), (texOriginalSize.y / 2) - (e.y / 2));


		// Normalized coordinates of pixel (10,10) in a 256x256 texture.
		ImVec2 uv0 = ImVec2(startPoint.x / texOriginalSize.x, startPoint.y / texOriginalSize.y);

		// Normalized coordinates of pixel (110,210) in a 256x256 texture.
		ImVec2 uv1 = ImVec2(endPoint.x / texOriginalSize.x, endPoint.y / texOriginalSize.y);

		// Display the 100x200 section starting at (10,10)
		ImGui::Image((ImTextureID)app->renderer3D->texColorBuffer, e, uv0, uv1);
	}
	ImGui::End();
}
