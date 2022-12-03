#pragma once
#include "Tab.h"
#include <string>
#include <vector>

class GameObject;

enum class ItemType{
	NONE = 0,
	TAG = 1,
	LAYER = 2
};

class Inspector : public Tab
{
public:
	Inspector();
	virtual ~Inspector() {};

	void Draw() override;

	void CalculateMaxWidth(std::vector<std::string> list, int &width);

	GameObject* gameObjectSelected = nullptr;

private:
	std::string DrawList(const char* label, std::vector<std::string>* list, std::string &item, int width);

	void AddItem(const char* label);

	void DrawDefaultInspector();
	void DrawEditLists();
	void DrawListTagLayer(const char* label, std::vector<std::string> list);

	void AddTag(std::string newTag);
	void AddLayer(std::string newLayer);

	std::vector<std::string> tags;
	std::vector<std::string> layers;

	
	int maxWidthTag = 125;
	int maxWidthLayers = 125;

	char newTag[16] = "";
	char newLayer[16] = "";
	ItemType item = ItemType::NONE;
};