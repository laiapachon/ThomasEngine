#pragma once

#include "Tab.h"

#include "ImGuiColorTextEdit/TextEditor.h"

class Shader;

class ShaderEditor : public Tab
{
public:
	ShaderEditor();
	virtual ~ShaderEditor();

	void Draw() override;

	void SetShader(Shader* shader);

private:
	// The shader to edit
	Shader* shader;

	TextEditor textEditor;
	TextEditor::LanguageDefinition lang;


};