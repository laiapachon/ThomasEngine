#include "Application.h"
#include "Material.h"
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
	
	if (!CompareTextureId(app->scene->root, this->GetOwner(), texture->textureID))
		delete texture;
	texture = nullptr;
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		
		ImGui::Checkbox("Active ", &active);
		if (texture->textureID != -1) 
		{
			IMGUI_PRINT("Texture Width: ", "%i", texture->texWidth);
			IMGUI_PRINT("Texture Height: ", "%i", texture->texHeight);
			ImGui::Image((ImTextureID)texture->textureID, ImVec2(128, 128));
		}
		ImGui::Checkbox("View with checkers", &viewWithCheckers);
	}
}
int Material::GetTextureID()
{
	return (viewWithCheckers == false) ? (active == true) ? texture->textureID : -1 : app->renderer3D->checkersTexture;	
}

bool Material::CompareTextureId(GameObject* node, GameObject* owner, GLuint id)
{
	bool ret = false;
	int numChildrens = node->GetChildrens().size();

	if (numChildrens != 0)
	{
		for (int i = 0; i < numChildrens; i++)
		{
			for ( int j = i; j < numChildrens; j++)
			{
				if (node->GetChildrens()[j] == nullptr) i++;
			}
			if (i == numChildrens) break;

			
			if (node->GetChildrens()[i] != owner)
			{
				Material* material = static_cast<Material*>(node->GetChildrens()[i]->GetComponent(ComponentType::MATERIAL));
				
				if (material != nullptr && material->texture->textureID == id)
					ret = true;
				
				else if (node->GetChildrens()[i]->GetChildrens().size() != 0)
					ret = CompareTextureId(node->GetChildrens()[i], owner, id);
			}
			if (ret) break;
		}
	}
	return ret;
}