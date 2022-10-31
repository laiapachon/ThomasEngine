#pragma once
#include "Tab.h"
#include <vector>

#define FPS_MS_LOG_MAXLENGHT 100

class Configuration : public Tab
{
public:
	Configuration();
	~Configuration();

	void Update();
	void Draw() override;
	void PushBackLog(std::vector<float> *log, float current);

private:
	std::vector<float> fpsLog;
	std::vector<float> msLog;
	std::vector<float> memoryLog;
};