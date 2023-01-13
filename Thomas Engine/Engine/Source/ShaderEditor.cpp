#include "ShaderEditor.h"

#include "Shader.h"

#include <fstream>

ShaderEditor::ShaderEditor() : shader(nullptr)
{
	textEditor = TextEditor();
	lang = TextEditor::LanguageDefinition::GLSL();
}

ShaderEditor::~ShaderEditor()
{
}

void ShaderEditor::Draw()
{
	auto cpos = textEditor.GetCursorPosition();
	std::string title = "Shader Editor: " + shader->GetName();
	ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				auto textToSave = textEditor.GetText();
				shader->SetNewSourceCode(textToSave);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = textEditor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				textEditor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && textEditor.CanUndo()))
				textEditor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && textEditor.CanRedo()))
				textEditor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, textEditor.HasSelection()))
				textEditor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && textEditor.HasSelection()))
				textEditor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && textEditor.HasSelection()))
				textEditor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				textEditor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				textEditor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(textEditor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				textEditor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				textEditor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				textEditor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, textEditor.GetTotalLines(),
		textEditor.IsOverwrite() ? "Ovr" : "Ins",
		textEditor.CanUndo() ? "*" : " ",
		textEditor.GetLanguageDefinition().mName.c_str(), shader->GetPath().c_str());

	textEditor.Render("ShaderEditor");

	ImGui::End();

}

void ShaderEditor::SetShader(Shader* shader)
{
	this->shader = shader;

	textEditor.SetText(shader->GetSourceCode());

	//std::ifstream t(shader->GetPath().c_str());
	//if (t.good())
	//{
	//	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	//	textEditor.SetText(str);
	//}

}
