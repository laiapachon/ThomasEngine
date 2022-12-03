#include "Application.h"
#include "ConsoleTab.h"


ConsoleTab::ConsoleTab() : Tab()
{
	name = "Console";
}

void ConsoleTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		DrawAppBarConsole();
		winSize = ImGui::GetWindowSize();
		winSize.x -= 16;
		winSize.y -= 84;

		if (ImGui::BeginChild("##OutPutConsole", winSize))
		{
			LogMsg* charLog = nullptr;
			ImVec4 labelColor(0.f, 0.f, 0.f, 0.f);

			
			for (unsigned int i = 0; i < logs.size(); ++i)
			{
				charLog = &logs[i];
				char labelLevel = GetMsgType(charLog->logType, labelColor);
				ImGui::TextColored(labelColor, "[%c]", labelLevel); 
				ImGui::SameLine();
				ImGui::TextWrapped(charLog->msg.c_str()); 
				if (logs[i].prints > 1)
				{
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(std::to_string(charLog->prints).c_str()).x);
					ImGui::Text("%i", charLog->prints);
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}
void ConsoleTab::DrawAppBarConsole()
{
	ImGui::TextWrapped("Normal logs: %d	", normalLog);
	ImGui::SameLine();
	ImGui::TextWrapped("Warnings logs: %d	", warningLog);
	ImGui::SameLine();
	ImGui::TextWrapped("Errors logs: %d	", errorLog);
	offset = ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Clear").x - 7;
	ImGui::SetCursorPosX(offset);
	ImGui::SetCursorPosY(25);

	if (ImGui::Button("Clear"))
	{
		logs.clear();
	}
	ImGui::Separator();
}
char ConsoleTab::GetMsgType(LogType type, ImVec4& logColor)
{
	char ret = 'N';

	switch (type)
	{
	case LogType::L_NORMAL:
		ret = 'N';
		logColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		break;

	case LogType::L_WARNING:
		ret = 'W';
		logColor = ImVec4(1.f, 1.f, 0.f, 1.f);
		break;

	case LogType::L_ERROR:
		ret = 'E';
		logColor = ImVec4(1.f, 0.f, 0.f, 1.f);
		break;
	}

	return ret;
}

void ConsoleTab::AddLog(const char* msg, LogType type)
{
	if (logs.size() >= 1)
	{
		if (logs[logs.size() - 1].msg == msg)
		{
			++logs[logs.size() - 1].prints;
			return;
		}
	}

	switch (type)
	{
	case LogType::L_NORMAL:
		normalLog++;
		break;
	case LogType::L_WARNING:
		warningLog++;
		break;
	case LogType::L_ERROR:
		errorLog++;
		break;
	default:
		break;
	}

	PushBackLog(msg, type);
}
void ConsoleTab::PushBackLog(const char* msg, LogType type)
{
	if (logs.size() <= LOG_MAX_CAPACITY) logs.push_back(LogMsg(msg, type));
	else
	{
		logs.erase(logs.begin());
		logs.push_back(LogMsg(msg, type));
	}
}