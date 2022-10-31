#include "Application.h"
#include "ConsoleTab.h"

ConsoleTab::ConsoleTab() : Tab()
{
	name = "Console";
}

ConsoleTab::~ConsoleTab()
{
}

void ConsoleTab::Draw()
{	
	if (ImGui::Begin(name.c_str()))
	{
		// Draw button Clear by clear the console
		// Get the ContentRegionMax (position + width) and substract the size of the box by know the position where draw it
		offset = ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Clear").x - 7;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("Clear"))
		{
			logs.clear();
		}
		ImGui::SameLine();

		offset -= ImGui::CalcTextSize("Collapse").x + (ImGui::CalcTextSize("Clear").x / 2) - 7;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("Collapse"))
		{
			collapsed = !collapsed;
		}

		ImGui::Separator();
		// Get WindowSize and substract some pixels because:
		// In Axis Y are button bar and this cover the some logs
		// And in Axis X if we didn't substract the scroll is not visible
		winSize = ImGui::GetWindowSize();
		winSize.y -= 63;
		winSize.x -= 16;

		if (ImGui::BeginChild("##outPutConsole", winSize))
		{
			charLog = nullptr;
			ImVec4 labelColor(0.f, 0.f, 0.f, 0.f);

			for (unsigned int i = 0; i < logs.size(); ++i)
			{
				charLog = &logs[i];
				// Get message type by know if this is Normal, Warning or Error
				char labelLevel = GetMsgType(charLog->logType, labelColor);

				ImGui::TextColored(labelColor, "[%c]", labelLevel);

				ImGui::SameLine();
				ImGui::TextWrapped(charLog->msg.c_str());
				// Print all logs
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
// Get message type and according this the text will be one color or another
char ConsoleTab::GetMsgType(LogType type, ImVec4& lColor)
{
	char ret = 'N';

	switch (type)
	{
	case LogType::L_NORMAL:
		ret = 'N';
		lColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		break;

	case LogType::L_WARNING:
		ret = 'W';
		lColor = ImVec4(1.f, 1.f, 0.f, 1.f);
		break;

	case LogType::L_ERROR:
		ret = 'E';
		lColor = ImVec4(1.f, 0.f, 0.f, 1.f);
		break;
	}

	return ret;
}

void ConsoleTab::AddLog(const char* s_msg, LogType _type)
{
	if (collapsed && logs.size() >= 1)
	{
		if (logs[logs.size() - 1].msg == s_msg)
		{
			++logs[logs.size() - 1].prints;
			return;
		}
	}

	logs.push_back(LogMsg(s_msg, _type));
}
