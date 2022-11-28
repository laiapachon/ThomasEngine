#include "Application.h"
#include "Editor.h"

// Modules
#include "Scene.h"
#include "GameObject.h"
#include "Window.h"
#include "Input.h"
#include "Camera3D.h"

// Tabs
#include "Tab.h"
#include "Configuration.h"
#include "ConsoleTab.h"
#include "AboutTab.h"
#include "Inspector.h"
#include "SceneTab.h"
#include "Hierarchy.h"
#include "GameTab.h"

#include "Primitive.h"

Editor::Editor(Application* app, bool start_enabled): Module(app, start_enabled)
{	
	name = "Editor";

	// Define size of the vector
	tabs = std::vector<Tab*>(static_cast<unsigned int>(TabType::MAX), nullptr);

	
	tabs[static_cast<unsigned int>(TabType::ABOUT)] = new AboutTab();
	tabs[static_cast<unsigned int>(TabType::CONSOLE)] = new ConsoleTab();
	tabs[static_cast<unsigned int>(TabType::CONFIGURATION)] = new Configuration();
	tabs[static_cast<unsigned int>(TabType::INSPECTOR)] = new Inspector();
	tabs[static_cast<unsigned int>(TabType::HIERARCHY)] = new Hierarchy(app->scene);
	tabs[static_cast<unsigned int>(TabType::SCENE)] = new SceneTab();
	tabs[static_cast<unsigned int>(TabType::GAME)] = new GameTab();

	// Assign a shortcut to each tab
	for (int i = 0; i < tabs.size(); i++)
	{
		tabs[i]->shortcut = i + 1;
	}
}

bool Editor::Init()
{
	LOG(LogType::L_NORMAL, "Init editor gui with imgui lib version %s", ImGui::GetVersion());

	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	
	
	/*ImGui::StyleColorsDark();*/
	ImGui::StyleColorsClassic();

	
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
	ConsoleTab* consoleWindow = static_cast<ConsoleTab*>(GetTab(TabType::CONSOLE));

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

	DockSpaceOverViewportCustom(viewport, ImGuiDockNodeFlags_PassthruCentralNode, dockPos, dockSize, nullptr);
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
	CheckShortCuts();
}

void Editor::CheckShortCuts()
{
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_UP)
		app->scene->CreateNewGameObject("GameObject");
	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_UP)
		app->scene->CreateChildGameObject("ChildGameObject", GetGameObjectSelected());
	else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_G) == KEY_UP)
		app->scene->CreateParentGameObject("ParentGameObject", GetGameObjectSelected());
	else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_UP) warningTab = true;
}

update_status Editor::Draw()
{	
	update_status ret = update_status::UPDATE_CONTINUE;

	StartFrame();
	ret = ImGuiMenuBar();
	CreateDockSpace();

	// Rendering the tabs
	for (unsigned int i = 0; i < tabs.size(); i++)
	{
		if (tabs[i]->active)
		{
			tabs[i]->Draw();
		}
	}
	if (warningTab)
		if (DrawWarningTab("New Scene")) NewScene();
	if (app->input->GetQuit())
		if (DrawWarningTab("Exit Engine")) ret = UPDATE_STOP;

	ImGui::EndFrame();
	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
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

bool Editor::DrawWarningTab(std::string text)
{
	bool ret = false;
	if (ImGui::Begin("Warning"))
	{
		float offset = ImGui::GetWindowContentRegionMax().x/2 - ImGui::CalcTextSize(text.c_str()).x/2;
		ImGui::SetCursorPosX(offset);
		ImGui::Text(text.c_str());

		ImGui::NewLine();
		ImGui::TextWrapped("Are you sure you want to %s?", text.c_str());
		ImGui::TextWrapped("If you close the scene, your changes will be lost. Do you want continue anyway?");
		ImGui::NewLine();

		offset = ImGui::GetWindowContentRegionMax().x / 2 - ImGui::CalcTextSize("YES").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("YES"))
		{
			ret = true;
			warningTab = false;
		}
		ImGui::SameLine();
		offset = ImGui::GetWindowContentRegionMax().x / 2 + ImGui::CalcTextSize("NO").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("NO"))
		{
			warningTab = false;
			app->input->SetQuit(false);
		}
	}
	ImGui::End();
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

			if (ImGui::MenuItem("New Scene","Ctrl+N"))
				warningTab = true;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			for (int i = 0; i < tabs.size(); i++)
			{
				// Name  ShortCut  State
				if (ImGui::MenuItem(tabs[i]->name.c_str(), std::to_string(tabs[i]->shortcut).c_str(), tabs[i]->active, &tabs[i]->active))
					tabs[i]->active = !tabs[i]->active;
			}			
			ImGui::EndMenu();
		}
		// Menu of game objects
		if (ImGui::BeginMenu("Game Objects"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				app->scene->CreateNewCamera();
			}
			if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N"))
			{
				app->scene->CreateNewGameObject("GameObject");
			}
			if (ImGui::MenuItem("Create Child", "Alt+Shift+N"))
			{
				app->scene->CreateChildGameObject("GameObjectChild", GetGameObjectSelected());
			}
			if (ImGui::MenuItem("Create Parent", "Ctrl+Shift+G"))
			{
				app->scene->CreateParentGameObject("GameObjectParent", GetGameObjectSelected());
			}
			if (ImGui::BeginMenu("3D Object"))
			{
				PrimitiveMenuItem();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
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

void Editor::NewScene()
{
	static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected = nullptr;
	app->scene->CleanUp(); //Clean GameObjects 
	app->scene->Init();
	app->camera->ReStartCamera();
}

void Editor::PrimitiveMenuItem()
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
}

bool Editor::CleanUp()
{
	bool ret= true;
	LOG(LogType::L_NORMAL, "Editor CleanUp");

	// CleanUp all tabs
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	RELEASE_VECTOR(tabs, tabs.size());
	tabs.clear();

	return ret;
}

Tab* Editor::GetTab(TabType type)
{
	unsigned int vecPosition = static_cast<unsigned int>(type);

	return (vecPosition < tabs.size()) ? tabs[vecPosition] : tabs[vecPosition];
}

GameObject* Editor::GetGameObjectSelected()
{
	Inspector* inspector = static_cast<Inspector*>(GetTab(TabType::INSPECTOR));
	return inspector->gameObjectSelected;
}


void Editor::DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspaceFlags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* windowClass)
{
	ImGuiWindowFlags host_window_flags = 0;
	host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;// | ImGuiWindowFlags_MenuBar;

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		host_window_flags |= ImGuiWindowFlags_NoBackground;

	char label[32];
	ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.5f, 0.5f));

	ImGui::Begin(label, NULL, host_window_flags);

	ImGui::PopStyleVar(3);
	ImGuiID dockspaceId = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags, windowClass);
	ImGui::End();
}