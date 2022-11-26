#pragma once
#include "Tab.h"
#include <vector>

#define LOG_MAX_CAPACITY 1000

struct LogMsg
{
	LogMsg(std::string msg, LogType type) : msg(msg), logType(type), prints(1) {};

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
	~ConsoleTab() { logs.clear(); };

	void Draw() override;

	void DrawAppBarConsole();

	void AddLog(const char* msg, LogType type);
	void PushBackLog(const char* msg, LogType type);
	char GetMsgType(LogType type, ImVec4& logColor);

private:
	ImVec2 winSize = {};
	std::vector<LogMsg> logs;
	float offset = 0.f;

	int normalLog = 0;
	int warningLog = 0;
	int errorLog = 0;
};

