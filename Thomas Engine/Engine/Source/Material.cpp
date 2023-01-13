#include "Application.h"
#include "Material.h"
#include "Scene.h"
#include "Renderer3D.h"
#include "ResourceTexture.h"
#include "Editor.h"
#include"Camera3D.h"
#include "ResourceManager.h"

#include "ShaderEditor.h"

#include "ImGui/imgui.h"

Material::Material(GameObject* obj) : Component(obj)
{
	shader = new Shader("default.shader");
	//shader = new Shader("water.shader");
	textEditor = new ShaderEditor();
	textEditor->SetShader(shader);
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

	if (ImGui::Button("Show shader editor"))
	{
		textEditor->active = true;
	}
	textEditor->Draw();

	if (ImGui::Button("Change shader"))
	{
		showShadersWindow = true;
	}
	if (showShadersWindow)
	{
		ImGui::Begin("Shaders", &showShadersWindow);
		std::map<std::string, Shader*> map = app->resourceManager->GetShadersMap();

		std::map<std::string, Shader*>::iterator it;
		for (it = map.begin(); it != map.end(); it++)
		{
			if (ImGui::Button(it->first.c_str()))
			{
				shader = it->second;
				break;
			}
		}

		ImGui::End();

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

void Material::Bind() 
{
	if (this == nullptr) 
	{
		return;
	}

	shader->Bind();
	shader->SetUniformMatrix4f("view", app->camera->GetViewMatrix().Transposed());
	Transform* model= (Transform*)owner->GetComponent(ComponentType::TRANSFORM);
	shader->SetUniformMatrix4f("model", model->GetLocalTransform().Transposed());
	shader->SetUniformMatrix4f("projection", app->camera->GetProjectionMatrix().Transposed());

	shader->SetUniform1f("waveSpeed", 5);
	shader->SetUniform1f("waveSize", 2);
	shader->SetUniform1f("waterHeight", 3);

	appTime += app->GetDt();
	shader->SetUniform1f("time",appTime);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->textureID);

	shader->SetUniform1i("tex",0);
}