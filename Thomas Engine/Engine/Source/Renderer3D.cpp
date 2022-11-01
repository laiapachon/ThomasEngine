#include "Application.h"
#include "Renderer3D.h"
#include "Globals.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GPUDetected/DeviceId.h"

#include "ResourceTexture.h"
#include "MeshRenderer.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

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

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
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
		LOG(LogType::L_NORMAL, "Init: Glew %s", glewGetString(GLEW_VERSION));
	}

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			LOG(LogType::L_ERROR, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		// Blend for transparency
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

	//Generate texture
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

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Generate scene buffers
	ReGenerateFrameBuffer(app->window->GetWindowWidth(), app->window->GetWindowHeight());

	// Projection matrix for
	OnResize(app->window->GetWindowWidth(), app->window->GetWindowHeight());

	// Load Primitives Test
	/*cube.InnerMesh();
	cube.mesh->LoadToMemory();*/

	//sphere.InnerMesh();
	//sphere.LoadToMemory();

	//cylinder.InnerMesh();
	//cylinder.LoadToMemory();

	//pyramid.InnerMesh();
	//pyramid.LoadToMemory();

	return ret;
}

// PreUpdate: clear buffer
update_status Renderer3D::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status Renderer3D::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	//glClearColor(0.f, 0.f, 0.f, 1.f);
	//glClear(GL_COLOR_BUFFER_BIT);
	// Axis and grid

	PrimitivePlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();


	// Comprobe wireframe mode
	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	(wireframe) ? glColor3f(Yellow.r, Yellow.g, Yellow.b) : glColor3f(White.r, White.g, White.b);

	// Draw all meshes
	if (!renderQueue.empty())
	{
		for (size_t i = 0; i < renderQueue.size(); i++)
		{
			renderQueue[i]->RenderMesh();
		}
		renderQueue.clear();
	}
	//cube.mesh->RenderMesh();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Draw all tabs
	ret = app->editor->Draw();

	SDL_GL_SwapWindow(app->window->window);

	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return ret;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Destroying 3D Renderer");

	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteRenderbuffers(1, &rbo);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);

	for (unsigned int k = 0; k < globalTextures.size(); ++k)
	{
		glDeleteTextures(1, &globalTextures[k]->textureID);
		delete globalTextures[k];
		globalTextures[k] = nullptr;
	}
	globalTextures.clear();

	for (unsigned int i = 0; i < globalMeshes.size(); i++)
	{
		delete globalMeshes[i];
		globalMeshes[i] = nullptr;
	}
	globalMeshes.clear();

	SDL_GL_DeleteContext(context);

	return true;
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

void Renderer3D::ReGenerateFrameBuffer(int w, int h)
{
	if (framebuffer > 0)
		glDeleteFramebuffers(1, &framebuffer);

	if (texColorBuffer > 0)
		glDeleteTextures(1, &texColorBuffer);

	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);


	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG(LogType::L_ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	App->window->SetSize(width, height);

	ReGenerateFrameBuffer(width, height);
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

		//ImGui::TextWrapped("All external library versions can be found in the 'About' window with links to their pages.");

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

		ImGui::SameLine();

		if (ImGui::Checkbox("GL_COLOR_MATERIAL", &colorMaterial)) {
			if (colorMaterial) glEnable(GL_COLOR_MATERIAL);
			else glDisable(GL_COLOR_MATERIAL);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable/Disable GL_COLOR_MATERIAL");

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

	}
}

bool Renderer3D::SaveConfig(JsonParser& node) const
{

	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "vsync", vsync);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "wireframe", wireframe);

	return true;
}

bool Renderer3D::LoadConfig(JsonParser& node)
{

	vsync = node.JsonValToBool("vsync");
	wireframe = node.JsonValToBool("wireframe");

	return true;
}