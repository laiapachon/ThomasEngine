#include "Application.h"
#include "Material.h"

#include"Renderer3D.h"
#include"ResourceTexture.h"

#include "ImGui/imgui.h"

Material::Material(GameObject* obj) : Component(obj), viewWithCheckers(false), matTexture(nullptr)
{
}

Material::~Material()
{
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (matTexture->textureID != -1) 
		{
			// Print Texture information, the Image and view model with checkers
			IMGUI_PRINT("Texture Width: ", "%i", matTexture->texWidth);
			IMGUI_PRINT("Texture Height: ", "%i", matTexture->texHeight);
			ImGui::Image((ImTextureID)matTexture->textureID, ImVec2(128, 128));
			ImGui::Checkbox("View with checkers", &viewWithCheckers);
		}
	}
}
// Get Texture ID from the image, this ID is unique
int Material::GetTextureID()
{
	return (viewWithCheckers == false) ? matTexture->textureID : app->renderer3D->checkersTexture;
}