#pragma once
#include "Tab.h"
#include <vector>

#define FPS_MS_LOG_MAXLENGHT 100


struct LogMsg
{
	LogMsg(std::string _msg, LogType _type) : msg(_msg), prints(1), logType(_type) {};

	std::string msg;
	LogType logType;
	unsigned int prints;

	bool operator==(const char* _msg)
	{
		return (strcmp(msg.c_str(), _msg) == 0) ? true : false;
	}
};

class ConsoleTab : public Tab
{
public:
	ConsoleTab();

	~ConsoleTab();

	void Draw() override;

	void AddLog(const char*, LogType);
	char GetMsgType(LogType, ImVec4&);

public:

	bool collapsed = true;
	ImVec2 winSize = { 200,400 };
	LogMsg* charLog;
	std::vector<LogMsg> logs;

private:
	float offset;
};

