#include "Application.h"
#include "ResourceManager.h"

#include "Scene.h"
#include "Editor.h"

#include "FileSystem.h"
#include "TextureLoader.h"
#include "MeshLoader.h"
#include "ModelImporter.h"

#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "Inspector.h" 
#include "Material.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "ResourceManager";
}

bool ResourceManager::Init()
{
	FileSystem::FSInit();

	return true;
}

bool ResourceManager::Start()
{
	logo = new Texture("logo.png", "logo");
	logo->LoadToMemory();

	backButton = new Texture("icon_back.png", "backButton");
	backButton->LoadToMemory();

	addButton = new Texture("icon_add.png", "addButton");
	addButton->LoadToMemory();

	return true;
}

bool ResourceManager::CleanUp()
{
	RELEASE(logo);
	RELEASE(backButton);
	RELEASE(addButton);

	FileSystem::FSDeInit();
	return true;
}

void ResourceManager::ImportFile(const char* assetsFile)
{
	std::string normalizedPath = FileSystem::NormalizePath(assetsFile);

	std::string output = "";

	char* buffer = nullptr;
	uint size = FileSystem::FileLoad(normalizedPath.c_str(), &buffer);

	if (buffer != nullptr && size != 0)
	{
		switch (FileSystem::GetImportType(assetsFile))
		{
		case ImportType::NOTYPE:
		{
			LOG(LogType::L_ERROR, "File extension not supported");
			break;
		}
		case ImportType::MESH:
		{
			ModelImporter::Import(normalizedPath.c_str(), buffer, size, app->scene->root);
			break;
		}
		case ImportType::TEXTURE:
		{
			Texture* material = new Texture(normalizedPath.c_str());
			if (FileSystem::FileExist(material->GetLibraryPath()))
			{
				overwritting = true;
				ovResource = material;
				ovBuffer = buffer;
				ovPath = material->GetLibraryPath();
			}
			else
			{
				material->Import(buffer, size, material->GetLibraryPath());
				material->LoadToMemory();

				Inspector* inspector = static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR));
				if (inspector && inspector->gameObjectSelected) {
					Material* mat = static_cast<Material*>(inspector->gameObjectSelected->GetComponent(ComponentType::MATERIAL));
					if (mat) mat->texture = material;
					else
					{
						Material* mat = static_cast<Material*>(inspector->gameObjectSelected->AddComponent(ComponentType::MATERIAL));
						mat->texture = material;
					}
				}
			}
			break;
		}
		default:
			break;
		}
		if (!overwritting) RELEASE_ARRAY(buffer);
	}
}

void ResourceManager::Overwrite()
{
	switch (ovResource->GetType())
	{
	case ResourceType::MESH:
	{
		ModelImporter::Import(ovPath, ovBuffer, ovSize, app->scene->root);
		break;
	}
	case ResourceType::TEXTURE:
	{
		Texture* material = (Texture*)ovResource;
		material->Import(ovBuffer, ovSize, ovPath);

		ovResource->LoadToMemory();

		Inspector* inspector = static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR));
		if (inspector && inspector->gameObjectSelected) {
			Material* mat = static_cast<Material*>(inspector->gameObjectSelected->GetComponent(ComponentType::MATERIAL));
			if (mat) mat->texture = material;
			else
			{
				Material* mat = static_cast<Material*>(inspector->gameObjectSelected->AddComponent(ComponentType::MATERIAL));
				mat->texture = material;
			}
		}
		break;
	}
	default:
		break;
	}

	overwritting = false;
	RELEASE_ARRAY(ovBuffer);
}

void ResourceManager::DrawOverwriteTab()
{
	if (ImGui::Begin("Warning"))
	{
		float offset = ImGui::GetWindowContentRegionMax().x / 2 - ImGui::CalcTextSize("Override file").x / 2;
		ImGui::SetCursorPosX(offset);
		ImGui::Text("Overwrite file");
		ImGui::NewLine();
		ImGui::TextWrapped("Are you sure you want to overwrite %s?", ovPath);
		ImGui::TextWrapped("You can create a new file with a counter on it's name.");
		ImGui::NewLine();

		offset = ImGui::GetWindowContentRegionMax().x / 3 - ImGui::CalcTextSize("OVERWRITE").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("OVERWRITE"))
		{
			Overwrite();
			overwritting = false;
		}
		ImGui::SameLine();
		offset = ImGui::GetWindowContentRegionMax().x / 3 + ImGui::CalcTextSize("CANCEL").x / 2;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("CREATE NEW"))
		{
			NewCounterFile();
			overwritting = false;
		}
		ImGui::SameLine();
		offset = ImGui::GetWindowContentRegionMax().x / 3 * 2 + ImGui::CalcTextSize("CANCEL").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("CANCEL"))
		{
			overwritting = false;
		}
	}
	ImGui::End();
}

void ResourceManager::NewCounterFile()
{
	std::string num = "_";
	std::string path(ovPath);
	std::string ext = path.substr(path.find_last_of("."));
	path = path.substr(0, path.find_last_of(".")).c_str();
	std::string resetter = path.c_str();
	std::string tester = path;
	for (int i = 1; !num.compare("_"); i++)
	{
		path = path + "_" + std::to_string(i) + ext;
		if (!FileSystem::FileExist(path.c_str()))
		{
			ovPath = path.c_str();
			break;
		}
		path = resetter;
	}
	Overwrite();
}