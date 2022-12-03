#include "Application.h"

// Module
#include "Renderer3D.h"
#include "Window.h"
#include "Globals.h"
#include "Camera3D.h"
#include "Editor.h"
#include "Scene.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GPUDetected/DeviceId.h"

#include <map>
#include "Geometry/Triangle.h"
#include "ResourceTexture.h"
#include "MeshRenderer.h"
#include "Transform.h"

#include "AboutTab.h"

#pragma comment (lib, "glu32.lib")    
#pragma comment (lib, "opengl32.lib")

Renderer3D::Renderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), vsync(false)
{
	name = "Renderer3D";

	GetCaps(hardware.caps);

	SDL_version version;
	SDL_GetVersion(&version);
	hardware.SDLVersion = std::to_string(version.major) + '.' + std::to_string(version.minor) + '.' + std::to_string(version.patch);
	hardware.CPUCount = SDL_GetCPUCount();
	hardware.CPUCache = SDL_GetCPUCacheLineSize();
	hardware.systemRAM = SDL_GetSystemRAM() / 1024.f;

	uint vendor, deviceId;
	std::wstring brand;
	unsigned __int64 videoMemBudget;
	unsigned __int64 videoMemUsage;
	unsigned __int64 videoMemAvailable;
	unsigned __int64 videoMemReserved;

	if (getGraphicsDeviceInfo(&vendor, &deviceId, &brand, &videoMemBudget, &videoMemUsage, &videoMemAvailable, &videoMemReserved))
	{
		hardware.GPUVendor = vendor;
		hardware.GPUDevice = deviceId;
		sprintf_s(hardware.GPUBrand, 250, "%S", brand.c_str());
		hardware.VRAMBudget = float(videoMemBudget) / (1073741824.0f / 1024.f);
		hardware.VRAMUsage = float(videoMemUsage) / (1024.f * 1024.f * 1024.f);
		hardware.VRAMAvailable = float(videoMemAvailable) / (1024.f * 1024.f);
		hardware.VRAMReserved = float(videoMemReserved) / (1024.f * 1024.f * 1024.f);
	}
}

bool Renderer3D::Init()
{
	LOG(LogType::L_NORMAL, "Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG(LogType::L_ERROR, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG(LogType::L_ERROR, "Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG(LogType::L_NORMAL, "Init Glew");
	}

	if (ret == true)
	{
		static_cast<AboutTab*>(app->editor->GetTab(TabType::ABOUT))->LogVersionDependences();

		if (VSYNC && SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			LOG(LogType::L_ERROR, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	for (int i = 0; i < SQUARE_TEXTURE_W; i++) {
		for (int j = 0; j < SQUARE_TEXTURE_H; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersTexture);
	glBindTexture(GL_TEXTURE_2D, checkersTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SQUARE_TEXTURE_W, SQUARE_TEXTURE_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	OnResize(app->window->GetWindowWidth(), app->window->GetWindowHeight());
	plane.SetPos(0, 0, 0);
	plane.constant = 0;
	plane.axis = true;

	return ret;
}

update_status Renderer3D::PreUpdate(float dt)
{
	app->camera->cameraScene.PreUpdate();

	lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

update_status Renderer3D::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	//glClearColor(0.f, 0.f, 0.f, 1.f);
	//glClear(GL_COLOR_BUFFER_BIT);

	plane.Render();

	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	(wireframe) ? glColor3f(Yellow.r, Yellow.g, Yellow.b) : glColor3f(White.r, White.g, White.b);

	if (!renderQueue.empty())
	{
		for (size_t i = 0; i < renderQueue.size(); i++)
		{
			renderQueue[i]->RenderMesh();
		}
	}

	DrawRay();

	app->camera->cameraScene.PostUpdate();

	if (app->scene->mainCamera != nullptr)
	{
		app->scene->mainCamera->PreUpdate();

		lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

		for (uint i = 0; i < MAX_LIGHTS; ++i)
			lights[i].Render();

		plane.Render();

		if (!renderQueue.empty())
		{
			for (size_t i = 0; i < renderQueue.size(); i++)
			{
				if (app->scene->mainCamera->frustrum.Intersects(renderQueue[i]->globalOBB))
					renderQueue[i]->RenderMesh();
			}
		}

		app->scene->mainCamera->PostUpdate();
	}

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ret = app->editor->Draw();

	renderQueue.clear();

	SDL_GL_SwapWindow(app->window->window);

	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return ret;
}

bool Renderer3D::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Destroying 3D Renderer");

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	SDL_GL_DeleteContext(context);
	RELEASE(plane.mesh);
	renderQueue.clear();

	return true;
}

void Renderer3D::DrawRay()
{
	if (viewRay)
	{
		glColor3f(1.f, 0.f, 0.f);
		glLineWidth(2.f);
		glBegin(GL_LINES);
		glVertex3fv(&app->camera->ray.a.x);
		glVertex3fv(&app->camera->ray.b.x);
		glEnd();
		glLineWidth(1.f);
		glColor3f(1.f, 1.f, 1.f);
	}
}

void Renderer3D::GetCaps(std::string& caps)
{
	caps += (SDL_HasRDTSC()) ? "RDTSC," : "";
	caps += (SDL_HasMMX()) ? "MMX, " : "";
	caps += (SDL_HasSSE()) ? "SSE, " : "";
	caps += (SDL_HasSSE2()) ? "SSE2, " : "";
	caps += (SDL_HasSSE3()) ? "SSE3, " : "";
	caps += "\n";
	caps += (SDL_HasSSE41()) ? "SSE41, " : "";
	caps += (SDL_HasSSE42()) ? "SSE42, " : "";
	caps += (SDL_HasAVX()) ? "AVX, " : "";
	caps += (SDL_HasAltiVec()) ? "AltiVec, " : "";
	caps += (SDL_Has3DNow()) ? "3DNow, " : "";
}

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	app->camera->cameraScene.ReGenerateFrameBuffer(width, height);
	if (app->scene->mainCamera != nullptr) 
	{
		app->scene->mainCamera->ReGenerateFrameBuffer(width, height);
	}
}

void Renderer3D::OnGUI()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{
		IMGUI_PRINT("SDL Version: ", "%s", hardware.SDLVersion.c_str());
		IMGUI_PRINT("OpenGL Version: ", "%s", glGetString(GL_VERSION));
		IMGUI_PRINT("Glew Version: ", "%s", glewGetString(GLEW_VERSION));

		ImGui::Separator();
		IMGUI_PRINT("GLSL: ", "%s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		IMGUI_PRINT("Vendor: ", "%s", glGetString(GL_VENDOR));
		IMGUI_PRINT("Renderer: ", "%s", glGetString(GL_RENDERER));

		ImGui::Separator();
		IMGUI_PRINT("CPUs: ", "%d (Cache: %dkb)", hardware.CPUCount, hardware.CPUCache);
		IMGUI_PRINT("System RAM: ", "%.1fGb", hardware.systemRAM);
		IMGUI_PRINT("Caps: ", hardware.caps.c_str());

		ImGui::Separator();
		IMGUI_PRINT("GPU:", "vendor %u device %u", hardware.GPUVendor, hardware.GPUDevice);
		IMGUI_PRINT("Brand:", hardware.GPUBrand);
		IMGUI_PRINT("VRAM Budget:", "%.1f Mb", hardware.VRAMBudget);
		IMGUI_PRINT("VRAM Usage:", "%.1f Mb", hardware.VRAMUsage);
		IMGUI_PRINT("VRAM Available:", "%.1f Mb", hardware.VRAMAvailable);
		IMGUI_PRINT("VRAM Reserved:", "%.1f Mb", hardware.VRAMReserved);
	}
	if (ImGui::CollapsingHeader("Debug"))
	{
		if (ImGui::Checkbox("GL_DEPTH_TEST", &depthTest)) {
			if (depthTest) glEnable(GL_DEPTH_TEST);
			else glDisable(GL_DEPTH_TEST);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable/Disable GL_DEPTH_TEST");

		ImGui::SameLine();

		if (ImGui::Checkbox("GL_CULL_FACE", &cullFace)) {
			if (cullFace) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable/Disable GL_CULL_FACE");


		if (ImGui::Checkbox("GL_TEXTURE_2D", &texture2D)) {
			if (texture2D) glEnable(GL_TEXTURE_2D);
			else glDisable(GL_TEXTURE_2D);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable/Disable GL_TEXTURE_2D");

		ImGui::SameLine();

		if (ImGui::Checkbox("GL_LIGHTING", &lighting)) {
			if (lighting) glEnable(GL_LIGHTING);
			else glDisable(GL_LIGHTING);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable/Disable GL_LIGHTING");

		if (ImGui::Checkbox("RAY_CASTING  ", &viewRay))

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Enable/Disable can view ray");

		ImGui::SameLine();
		if (ImGui::Checkbox("GL_COLOR_MATERIAL", &colorMaterial)) {
			if (colorMaterial) glEnable(GL_COLOR_MATERIAL);
			else glDisable(GL_COLOR_MATERIAL);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable/Disable GL_COLOR_MATERIAL");


		if (ImGui::Checkbox("GL_FOG_WAR   ", &fog))
		{
			if (fog)
			{
				glEnable(GL_FOG);
				glFogfv(GL_FOG_COLOR, fogColor); 
				if (fogLinear)
				{
					glFogi(GL_FOG_MODE, GL_LINEAR); 
					glFogf(GL_FOG_START, fogStart);
					glFogf(GL_FOG_END, fogEnd);
				}
				else
				{
					glFogi(GL_FOG_MODE, GL_EXP);
					glFogf(GL_FOG_DENSITY, fogDensity);
				}
				float* newFarPlane = &app->scene->mainCamera->frustrum.farPlaneDistance;
				oldFarPlane = *newFarPlane;
				*newFarPlane = fogFarPlane;
				gluPerspective(45.0f, 800.0f / 600.0f, 1.0f, *newFarPlane);
				newFarPlane = nullptr;
			}
			else
			{
				glDisable(GL_FOG);
				app->scene->mainCamera->frustrum.farPlaneDistance = oldFarPlane;
			}
		}


		if (fog)
		{
			if (app->scene->mainCamera->frustrum.farPlaneDistance != fogFarPlane) fogFarPlane = app->scene->mainCamera->frustrum.farPlaneDistance;
			ImGui::Text("----FOG----");

			if (ImGui::Checkbox("Linear", &fogLinear) && fogExpo)
			{
				glFogi(GL_FOG_MODE, GL_LINEAR);
				fogExpo = false;
				fogLinear = true;
			}

			if (ImGui::Checkbox("Exponential", &fogExpo) && fogLinear)
			{
				glFogi(GL_FOG_MODE, GL_EXP);
				fogLinear = false;
				fogExpo = true;
			}

			if (ImGui::SliderFloat("Red", &fogColor[0], 0.0f, 1.0f))
				glFogfv(GL_FOG_COLOR, fogColor);

			if (ImGui::SliderFloat("Green", &fogColor[1], 0.0f, 1.0f));
			glFogfv(GL_FOG_COLOR, fogColor);

			if (ImGui::SliderFloat("Blue", &fogColor[2], 0.0f, 1.0f));
			glFogfv(GL_FOG_COLOR, fogColor);

			if (fogLinear)
			{
				if (ImGui::SliderFloat("Start", &fogStart, 10.0f, 39.0f));
				glFogf(GL_FOG_START, fogStart);

				if (ImGui::SliderFloat("End", &fogEnd, 40.0f, 80.0f));
				glFogf(GL_FOG_END, fogEnd);
			}

			else if (ImGui::SliderFloat("Density", &fogDensity, 0.0f, 1.0f));
			glFogf(GL_FOG_DENSITY, fogDensity);
		}
	}
}

bool Renderer3D::SaveConfig(JsonParser& node) const
{

	node.SetJBool(node.ValueToObject(node.GetRootValue()), "vsync", vsync);
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "wireframe", wireframe);

	return true;
}

bool Renderer3D::LoadConfig(JsonParser& node)
{

	vsync = node.JsonValToBool("vsync");
	wireframe = node.JsonValToBool("wireframe");

	return true;
}

void Renderer3D::MousePicking(LineSegment ray)
{
	float dNear = 0;
	float dFar = 0;
	std::map<float, MeshRenderer*> possibleHitList;

	for (std::vector<MeshRenderer*>::iterator it = renderQueue.begin(); it != renderQueue.end(); ++it)
	{
		if (ray.Intersects((*it)->globalOBB, dNear, dFar))
			possibleHitList[dNear] = (*it);
	}

	std::map<float, MeshRenderer*> hitList;

	for (std::map<float, MeshRenderer*>::const_iterator it = possibleHitList.begin(); it != possibleHitList.end(); ++it)
	{
		const Mesh* mesh = (*it).second->GetMesh();
		if (mesh != nullptr)
		{
			LineSegment localRay = ray;
			localRay.Transform((*it).second->GetOwner()->transform->GetGlobalTransform().Inverted());

			Triangle tri;
			for (uint i = 0; i < mesh->numIndexs; i += 3)
			{
				tri.a = float3(&mesh->vertex[mesh->indexs[i] * 3]);
				tri.b = float3(&mesh->vertex[mesh->indexs[i + 1] * 3]);
				tri.c = float3(&mesh->vertex[mesh->indexs[i + 2] * 3]);

				float dist = 0;
				if (localRay.Intersects(tri, &dist, nullptr))
				{
					hitList[dist] = (*it).second;
				}
			}
		}
	}

	bool objectHit = false;
	if (hitList.begin() != hitList.end())
	{
		App->editor->SetGameObjectSelected((*hitList.begin()).second->GetOwner());
		objectHit = true;
	}
	possibleHitList.clear();
	hitList.clear();

	if (!objectHit)
		App->editor->SetGameObjectSelected(nullptr);
}