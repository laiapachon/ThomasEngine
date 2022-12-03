#pragma once
#include "Module.h"
#include "Imgui/imgui.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP,
	SCROLL_UP,
	SCROLL_DOWN
};

class Input : public Module
{
public:
	
	Input(Application* app, bool start_enabled = true) ;
	~Input() { delete[] keyboard; };

	bool Init()override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetWheel() const
	{
		return wheel;
	}

	void GetMousePosition(int& x, int& y) const
	{
		x = mouse_x;
		y = mouse_y;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	void OnGUI() override;
	void AddInput(const char* entry);
	void LogInputEvent(uint key, uint state);

	bool GetQuit() { return quit; };
	void SetQuit(bool ret) { quit = ret; };

private:
	ImGuiTextBuffer inputBuf;
	bool need_scroll = false;

	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int wheel;
	int mouse_x_motion;
	int mouse_y_motion;
	bool quit = false;
};