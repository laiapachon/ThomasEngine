#pragma once

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class Window : public Module
{
public:

	Window(Application* app, bool start_enabled = true);
	virtual ~Window() {};

	bool Init()override;
	bool CleanUp()override;

	void SetTitle(const char* title);

	void SetBrightness(float bright);
	void SetBrightness();
	void SetSize(uint w, uint h);
	void SetSize();
	void SetFullscreen(bool fullscreen);
	void SetFullscreen();
	void SetResizable(bool isResizable);
	void SetResizable();
	void SetBorderless(bool borderless);
	void SetBorderless();
	void SetFullscreenDesktop(bool fullsdesktop);
	void SetFullscreenDesktop();
	bool IsFullscreen() const { return fullScreen; }
	bool IsResizable() const { return isResizable; }
	bool IsBorderless() const { return borderless; }
	bool IsFullscreenDesktop() const { return fullScreenDesktop; }

	float GetBrightness() const { return brightness; }
	void GetSize(int& h, int& w)const;

	void OnResize(int width, int height);

	update_status ManageEvent(SDL_Event* e);

	void OnGUI() override;

	bool SaveConfig(JsonParser& node) const override;

	bool LoadConfig(JsonParser& node) override;

	int GetWindowWidth() { return width; };
	int GetWindowHeight() { return height; };

public:
	//The window we'll be rendering to
	SDL_Window* window;
	SDL_GLContext gl_context;

	//The surface contained by the window
	SDL_Surface* screenSurface;

private:
	SDL_DisplayMode current;

	int width;
	int height;
	float brightness = 1;

	bool fullScreen = false;
	bool borderless = false;
	bool isResizable = false;
	bool fullScreenDesktop = false;
};