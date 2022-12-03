#include "AboutTab.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/imgui_user.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include"PhysFS/include/physfs.h"
#include "Assimp/include/version.h"

#include "SDL/include/SDL.h"

AboutTab::AboutTab()
{
	name = "About";
	active = false;
}

void AboutTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		ImGui::Text("Thomas Engine");
		ImGui::Text("This is a university project focused");
		ImGui::Text("on the development of a video game engine");

		ImGui::NewLine();
		ImGui::Text("Developed by: ");
		if (ImGui::MenuItem("Enric Morales"))
		{
			ShellExecute(0, 0, "https://github.com/enricmc19", 0, 0, SW_SHOW);
		}
		if (ImGui::MenuItem("Laia Pachon"))
		{
			ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine", 0, 0, SW_SHOW);
		}

		ImGui::NewLine();
		ImGui::Separator();
	
		ImGui::Text("3rd Party Libraries used:");
		IMGUI_PRINT("SDL Version: ", "%s", SDLVersion.c_str());
		IMGUI_PRINT("OpenGL Version: ", "%s", OGLVersion);
		IMGUI_PRINT("Glew Version: ", "%s", GlewVersion);
		IMGUI_PRINT("GLSL: ", "%s", GLSLVersion);
		IMGUI_PRINT("PhysFS Version: ", "%s", physVersion.c_str());
		IMGUI_PRINT("Assimp Version: ", "%s", assimpVersion.c_str());
		IMGUI_PRINT("ImGui Version: ", "%s", imGuiVersion);
		IMGUI_PRINT("MathGeoLib Version: ", "%s", mathGeoLib);
		IMGUI_PRINT("Parson Version: ", "%s", parsonVersion);
		IMGUI_PRINT("DeviL Version: ", "%s", deviLVersion);

		ImGui::NewLine();
		ImGui::Separator();
		PrintLicense();
	}
	ImGui::End();	
}

void AboutTab::PrintLicense()
{
	ImGui::Text("License:");
	ImGui::Text("MIT License \nCopyright(c) 2022 Enric Morales Laia Pachon \n");
	ImGui::NewLine();
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this softwareand associated documentation files (the \"Software\"), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and /or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions:");
	ImGui::NewLine();
	ImGui::Text("The above copyright noticeand this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::NewLine();
	ImGui::Text("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE");
}

void AboutTab::LogVersionDependences()
{
	InitVersions();

	LOG(LogType::L_NORMAL, "-------------- 3rd Party Libraries used --------------");
	LOG(LogType::L_NORMAL, "SDL Version: %s", SDLVersion.c_str());
	LOG(LogType::L_NORMAL, "OpenGL Version: %s", OGLVersion);
	LOG(LogType::L_NORMAL, "Glew Version: %s", GlewVersion);
	LOG(LogType::L_NORMAL, "GLSL: %s", GLSLVersion);
	LOG(LogType::L_NORMAL, "ImGui Version: %s", ImGui::GetVersion());
	LOG(LogType::L_NORMAL, "MathGeoLib Version: 1.5");
	LOG(LogType::L_NORMAL, "Parson Version: 1.2.1");
	LOG(LogType::L_NORMAL, "Assimp Version: %s", assimpVersion.c_str());
	LOG(LogType::L_NORMAL, "PhysFS Version: %s", physVersion.c_str());
	LOG(LogType::L_NORMAL, "DeviL Version: 1.8");
	LOG(LogType::L_NORMAL, "Vendor: %s", glGetString(GL_VENDOR));
	LOG(LogType::L_NORMAL, "Renderer: %s", glGetString(GL_RENDERER));
}

void AboutTab::InitVersions()
{
	PHYSFS_Version physVersionStc;
	PHYSFS_getLinkedVersion(&physVersionStc);
	SDL_version version;
	SDL_GetVersion(&version);
	SDLVersion = std::to_string(version.major) + '.' + std::to_string(version.minor) + '.' + std::to_string(version.patch);
	assimpVersion = std::to_string(aiGetVersionMajor()) + '.' + std::to_string(aiGetVersionMinor()) + '.' + std::to_string(aiGetVersionRevision());
	physVersion = std::to_string(physVersionStc.major) + '.' + std::to_string(physVersionStc.minor) + '.' + std::to_string(physVersionStc.patch);
	imGuiVersion = ImGui::GetVersion();
	OGLVersion = glGetString(GL_VERSION);
	GlewVersion = glewGetString(GLEW_VERSION);
	GLSLVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	mathGeoLib = "1.5";
	parsonVersion = "1.2.1";
	deviLVersion = "1.8";
}
