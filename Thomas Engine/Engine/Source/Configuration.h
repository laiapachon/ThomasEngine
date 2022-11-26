#pragma once
#include "Tab.h"
#include <vector>
#include "Globals.h"


class Configuration : public Tab
{
public:
	Configuration();
	~Configuration();

	void Draw() override;

	void UpdateLogs();
	void PushBackLog(std::vector<float> *log, float current);

private:
	std::vector<float> fpsLog;
	std::vector<float> msLog;
	std::vector<float> memoryLog;

	char nameEngine[24] = TITLE;
	char nameOrganization[24] = ORGANIZATION_NAME;

};