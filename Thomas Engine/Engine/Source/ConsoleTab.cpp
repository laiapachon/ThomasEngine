#include "Application.h"
#include "ConsoleTab.h"

// Module
//#include "Editor.h"

ConsoleTab::ConsoleTab() : Tab()
{
	name = "Console";
}

void ConsoleTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		DrawAppBarConsole();

		// Get WindowSize and substract some pixels because:
		// In Axis Y are button bar and this cover the some logs
		// And in Axis X if we didn't substract the scroll is not visible
		winSize = ImGui::GetWindowSize();
		winSize.x -= 16;
		winSize.y -= 84;

		if (ImGui::BeginChild("##OutPutConsole", winSize))
		{
			LogMsg* charLog = nullptr;
			ImVec4 labelColor(0.f, 0.f, 0.f, 0.f);

			// Print all logs
			for (unsigned int i = 0; i < logs.size(); ++i)
			{
				charLog = &logs[i];
				// Get message type by know if this is Normal, Warning or Error
				char labelLevel = GetMsgType(charLog->logType, labelColor);

				ImGui::TextColored(labelColor, "[%c]", labelLevel); // Type message
				ImGui::SameLine();
				ImGui::TextWrapped(charLog->msg.c_str()); // Message
				
				// If the log is repeated, instead of painting it underneath
				// Indicate it with the number of times it is repeated 
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
	// Print the number of log types
	ImGui::TextWrapped("Normal logs: %d	", normalLog);
	ImGui::SameLine();
	ImGui::TextWrapped("Warnings logs: %d	", warningLog);
	ImGui::SameLine();
	ImGui::TextWrapped("Errors logs: %d	", errorLog);

	// Draw button Clear by clear the console
	// Get the ContentRegionMax (position + width) and substract the size of the box by know the position where draw it
	offset = ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Clear").x - 7;
	ImGui::SetCursorPosX(offset);
	ImGui::SetCursorPosY(25);

	if (ImGui::Button("Clear"))
	{
		logs.clear();
	}
	ImGui::Separator();
}
// Get message type and according this the text will be one color or another
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
	// If the last log is equal at new log, prints++
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
// To limit the capacity of the console LOG_MAX_CAPACITY = 1000
void ConsoleTab::PushBackLog(const char* msg, LogType type)
{
	if (logs.size() <= LOG_MAX_CAPACITY) logs.push_back(LogMsg(msg, type));
	else
	{
		logs.erase(logs.begin());
		logs.push_back(LogMsg(msg, type));
	}
}