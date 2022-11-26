#include "Application.h"
#include "Material.h"


// Modules
#include "Scene.h"
#include "Renderer3D.h"
#include "ResourceTexture.h"
#include "Editor.h"

#include "ImGui/imgui.h"

Material::Material(GameObject* obj) : Component(obj)
{
}

Material::~Material()
{
	// Only delete texture if nobody is using it  
	if (!CompareTextureId(app->scene->root, this->GetOwner(), texture->textureID))
		delete texture;
	texture = nullptr;
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// This can't call "Active" because there is already another checkbox with that name
		// that's why it's called "Active " 
		ImGui::Checkbox("Active ", &active);
		if (texture->textureID != -1) 
		{
			// Print Texture information, the Image and view model with checkers
			IMGUI_PRINT("Texture Width: ", "%i", texture->texWidth);
			IMGUI_PRINT("Texture Height: ", "%i", texture->texHeight);
			ImGui::Image((ImTextureID)texture->textureID, ImVec2(128, 128));
		}
		ImGui::Checkbox("View with checkers", &viewWithCheckers);
	}
}
// Get Texture ID from the image, this ID is unique
int Material::GetTextureID()
{
	return (viewWithCheckers == false) ? (active == true) ? texture->textureID : -1 : app->renderer3D->checkersTexture;	
}

bool Material::CompareTextureId(GameObject* node, GameObject* owner, GLuint id)
{
	bool ret = false;
	int numChildrens = node->GetChildrens().size();

	// If don't have childrens stop recursive
	if (numChildrens != 0)
	{
		for (int i = 0; i < numChildrens; i++)
		{
			// Security system: The children that are null we want to check them 
			// Why are NULL? Because we can be in the cleanup process
			for ( int j = i; j < numChildrens; j++)
			{
				if (node->GetChildrens()[j] == nullptr) i++;
			}
			if (i == numChildrens) break;

			// Exclude the owner of the id from the check
			if (node->GetChildrens()[i] != owner)
			{
				Material* material = static_cast<Material*>(node->GetChildrens()[i]->GetComponent(ComponentType::MATERIAL));
				// Check if this gameObject has material and if so check if the id matches
				if (material != nullptr && material->texture->textureID == id)
					ret = true;
				// Otherwise check if this gameObject has childrens and the recursion begins
				else if (node->GetChildrens()[i]->GetChildrens().size() != 0)
					ret = CompareTextureId(node->GetChildrens()[i], owner, id);
			}
			if (ret) break;
		}
	}
	return ret;
}