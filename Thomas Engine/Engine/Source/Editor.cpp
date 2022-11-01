#include "Application.h"
#include "Editor.h"

#include "GameObject.h"
#include "MeshRenderer.h"

// Tabs
#include "Tab.h"
#include "Configuration.h"
#include "ConsoleTab.h"
#include "AboutTab.h"
#include "Inspector.h"
#include "SceneTab.h"
#include "Hierarchy.h"

#include "Primitive.h"

Editor::Editor(Application* app, bool start_enabled): Module(app, start_enabled)
{	
	name = "Editor";

	// Define size of the vector
	tabs = std::vector<Tab*>(static_cast<unsigned int>(TabType::MAX), nullptr);

	// Fill the array with the types of tabs 
	// The first must be AboutTab
	tabs[static_cast<unsigned int>(TabType::ABOUT)] = new AboutTab();
	tabs[static_cast<unsigned int>(TabType::CONSOLE)] = new ConsoleTab();
	tabs[static_cast<unsigned int>(TabType::CONFIGURATION)] = new Configuration();
	tabs[static_cast<unsigned int>(TabType::INSPECTOR)] = new Inspector();
	tabs[static_cast<unsigned int>(TabType::SCENE)] = new SceneTab();
	tabs[static_cast<unsigned int>(TabType::HIERARCHY)] = new Hierarchy(app->scene);

	// Assign a shortcut to each tab
	for (int i = 0; i < tabs.size(); i++)
	{
		tabs[i]->shortcut = i + 1;
	}
}

Editor::~Editor()
{
}

bool Editor::Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->window->gl_context);
	ImGui_ImplOpenGL3_Init();

    return true;
}

bool Editor::Start()
{
    return true;
}
void Editor::LogToConsole(const char* msg, LogType _type)
{
	ConsoleTab* consoleWindow = dynamic_cast<ConsoleTab*>(GetTab(TabType::CONSOLE));

	if (consoleWindow != nullptr)
		consoleWindow->AddLog(msg, _type);
}

void Editor::CreateDockSpace()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 dockPos(viewport->WorkPos);
	ImGui::SetNextWindowPos(dockPos);

	ImVec2 dockSize(viewport->WorkSize);
	ImGui::SetNextWindowSize(dockSize);

	dockId = DockSpaceOverViewportCustom(viewport, ImGuiDockNodeFlags_PassthruCentralNode, dockPos, dockSize, nullptr);
}

void Editor::StartFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Check if any key is pressed and disable/enable the tab
	for (int i = 0; i < tabs.size(); i++)
	{
		if (App->input->GetKey(29 + tabs[i]->shortcut) == KEY_UP)
		{
			tabs[i]->active = !tabs[i]->active;
		}
	}
}

update_status Editor::Draw()
{	
	StartFrame();

	// Rendering the tabs
	update_status ret = ImGuiMenuBar();
	CreateDockSpace();

	if (showCase)
	{
		//ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::ShowDemoWindow();
		//ImGui::PopStyleColor();
	}
	for (unsigned int i = 0; i < tabs.size(); i++)
	{
		if (tabs[i]->active)
		{
			tabs[i]->Draw();
		}
	}

	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
	return ret;
}

update_status Editor::ImGuiMenuBar()
{
	// Create a MenuBar
	update_status ret = update_status::UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ESC"))
				ret = UPDATE_STOP;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			for (int i = 0; i < tabs.size(); i++)
			{
				if (ImGui::MenuItem(tabs[i]->name.c_str(), std::to_string(i+1).c_str(), tabs[i]->active, &tabs[i]->active))
					tabs[i]->active =! tabs[i]->active;
			}			
			ImGui::EndMenu();
		}
		// Game Object menu
		if (ImGui::BeginMenu("Game Objects"))
		{
			if (ImGui::MenuItem("Cube", nullptr))
			{
				PrimitiveCube cubePrim = PrimitiveCube();
				cubePrim.InnerMesh();
				cubePrim.mesh->LoadToMemory();

				app->scene->CreatePrimitive("Cube", cubePrim.mesh);
			}
			if (ImGui::MenuItem("Sphere", nullptr))
			{
				PrimitiveSphere spherePrim = PrimitiveSphere();
				spherePrim.InnerMesh();
				spherePrim.mesh->LoadToMemory();

				app->scene->CreatePrimitive("Sphere", spherePrim.mesh);
			}
			if (ImGui::MenuItem("Cylinder", nullptr))
			{
				PrimitiveCylinder cylinderPrim = PrimitiveCylinder();
				cylinderPrim.InnerMesh();
				cylinderPrim.mesh->LoadToMemory();

				app->scene->CreatePrimitive("Cylinder", cylinderPrim.mesh);
			}
			if (ImGui::MenuItem("Pyramid", nullptr))
			{
				PrimitivePyramid pyramidPrim = PrimitivePyramid();
				pyramidPrim.InnerMesh();
				pyramidPrim.mesh->LoadToMemory();

				app->scene->CreatePrimitive("Pyramid", pyramidPrim.mesh);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("ImGui Demo", nullptr, showCase))
			{
				showCase = !showCase;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine/wiki", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Download latest"))
			{
				ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine/releases", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine/issues", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("About"))
			{
				tabs[0]->active = !tabs[0]->active;
			}			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	return ret;
}

bool Editor::CleanUp()
{
	bool ret= true;
	LOG(LogType::L_NORMAL, "Editor CleanUp");

	// CleanUp all tabs
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	for (unsigned int i = 0; i < tabs.size(); ++i)
	{
		delete tabs[i];
		tabs[i] = nullptr;
	}
	tabs.clear();

	return ret;
}

Tab* Editor::GetTab(TabType type)
{
	unsigned int vecPosition = static_cast<unsigned int>(type);

	return (vecPosition < tabs.size()) ? tabs[vecPosition] : nullptr;
}

GameObject* Editor::GetGameObjectSelected()
{
	Inspector* inspector = dynamic_cast<Inspector*>(GetTab(TabType::INSPECTOR));
	return inspector->gameObjectSelected;
}


ImGuiID Editor::DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspaceFlags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* windowClass)
{
	if (viewport == NULL)
		viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(position);
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags host_window_flags = 0;
	host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		host_window_flags |= ImGuiWindowFlags_NoBackground;

	char label[32];
	ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(label, NULL, host_window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspaceId = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags, windowClass);
	ImGui::End();

	return dockspaceId;
}