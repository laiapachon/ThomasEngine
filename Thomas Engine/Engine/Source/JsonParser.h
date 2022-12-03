#pragma once
#include "Parson/parson.h"


class JsonParser
{
public:
	JsonParser();
	~JsonParser() {};

	JSON_Value* InitJsonObject();

	JSON_Value* FileToValue(const char* fileName);

	JSON_Object* ValueToObject(JSON_Value* value) const;

	const char* JsonValToStr(const char* name) const;

	double JsonValToNumber(const char* name) const;

	bool JsonValToBool(const char* name) const;

	JsonParser GetChild(JSON_Value* parent, const char* name);

	bool ExistingChild(JSON_Value* parent, const char* name);

	JSON_Status FileSerialization(JSON_Value* value, const char* fileName) const;

	JSON_Value* GetRootValue() const;

	JSON_Object* SetJNode(JSON_Object* parent, const char* nodeName) const;

	JSON_Status SetJString(JSON_Object* node, const char* name, const char* string) const;

	JSON_Status SetJNumber(JSON_Object* node, const char* name, double number) const;

	JSON_Status SetJBool(JSON_Object* node, const char* name, bool boolean) const;

	JsonParser SetChild(JSON_Value* parent, const char* name);


private:
	JSON_Value* rootValue;


};

struct JsonConfig
{
	JsonConfig();
	JsonConfig(JSON_Object* _nObj);

	void WriteInt(const char* name, int value);
	int ReadInt(const char* name);

	JSON_Object* nObj;
	JSON_Value* root;
};