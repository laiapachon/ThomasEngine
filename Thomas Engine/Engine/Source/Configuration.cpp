#include "Application.h"
#include "Configuration.h"
#include "mmgr/mmgr.h"
#include "Renderer3D.h"
#include "Editor.h"


Configuration::Configuration() : Tab()
{
	name = "Configuration";

	fpsLog.reserve(LOG_MAX_LENGHT);
	msLog.reserve(LOG_MAX_LENGHT);
	memoryLog.reserve(LOG_MAX_LENGHT);
}

Configuration::~Configuration()
{
	msLog.clear();
	fpsLog.clear();
	memoryLog.clear();
}

void Configuration::Draw()
{
	UpdateLogs();

	if (ImGui::Begin(name.c_str(), &active))
	{
		if (ImGui::CollapsingHeader("Config Saver"))
		{
			if (ImGui::Button("Save"))app->SaveConfigRequest();
			ImGui::SameLine();
			if (ImGui::Button("Load"))app->LoadConfigRequest();
		}
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::InputText("App Name", nameEngine, IM_ARRAYSIZE(nameEngine));
			ImGui::InputText("Organization", nameOrganization, IM_ARRAYSIZE(nameEngine));
			if (ImGui::SliderInt("Max FPS", &app->maxFPS, 0, 144))
			{
				app->renderer3D->vsync = false;
			}
			
			IMGUI_PRINT("Limit Framerate: ", (app->maxFPS == 0) ? "VSync" : "%d", app->maxFPS);

			char title[20];
			sprintf_s(title, 20, "Framerate %.1f", fpsLog[fpsLog.size() - 1]);
			ImGui::PlotHistogram("##FrameRate", &fpsLog[0], fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 20, "Milliseconds %0.1f", msLog[msLog.size() - 1]);
			ImGui::PlotHistogram("##Miliseconds", &msLog[0], msLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			ImGui::NewLine();

			
			sMStats stats = m_getMemoryStatistics();
			PushBackLog(&memoryLog, stats.totalReportedMemory);

			ImGui::PlotHistogram("##memory", &memoryLog[0], memoryLog.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

			IMGUI_PRINT("Total Reported Mem: ", "%u", stats.totalReportedMemory);
			IMGUI_PRINT("Total Actual Mem: ", "%u", stats.totalActualMemory);
			IMGUI_PRINT("Peak Reported Mem: ", "%u", stats.peakReportedMemory);
			IMGUI_PRINT("Peak Actual Mem: ", "%u", stats.peakActualMemory);
			IMGUI_PRINT("Accumulated Reported Mem: ", "%u", stats.accumulatedReportedMemory);
			IMGUI_PRINT("Accumulated Actual Mem: ", "%u", stats.accumulatedActualMemory);
			IMGUI_PRINT("Accumulated Alloc Unit Count: ", "%u", stats.accumulatedAllocUnitCount);
			IMGUI_PRINT("Total Alloc Unit Count: ", "%u", stats.totalAllocUnitCount);
			IMGUI_PRINT("Peak Alloc Unit Count: ", "%u", stats.peakAllocUnitCount);
		}
		
		for (unsigned int i = 0; i < app->listModules.size(); ++i)
		{
			app->listModules[i]->OnGUI();
		}
	}
	ImGui::End();
}

void Configuration::UpdateLogs()
{
	float currentFPS = floorf(app->GetFrameRate());
	float currentMS = (1000.f * app->GetDt());

	PushBackLog(&fpsLog, currentFPS);
	PushBackLog(&msLog, currentMS);
}

void Configuration::PushBackLog(std::vector<float> *log, float current)
{
	if (log->size() <= LOG_MAX_LENGHT) log->push_back(current);
	else
	{
		log->erase(log->begin());
		log->push_back(current);
	}
}
