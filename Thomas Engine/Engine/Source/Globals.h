#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

//#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);


#define INVALID_OGL_VALUE 0xffffffff
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices )
#define NAME_ENGINE "Thomas Engine"
enum class LogType
{
	L_NORMAL,
	L_WARNING,
	L_ERROR,
	L_NO_PRINTABLE
};

#define LOG(_type, format, ...) log(__FILE__, __LINE__, _type, format, __VA_ARGS__);
void log(const char file[], int line, LogType _type, const char* format, ...);
//void log(const char file[], int line,const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI 3.14159265358979323846

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
	   }\
	}

// Deletes an std::vector
#define RELEASE_VECTOR( x, s )\
	{\
		for (size_t i = 0; i < s; i++)\
		{\
			delete x[i];\
			x[i] = nullptr;\
		}\
	}

// Folders defines
#define ASSETS_FOLDER "Assets/"
#define SETTINGS_FOLDER "Settings/"

#define LIBRARY_FOLDER "Library/"
#define MESHES_FOLDER "Library/Meshes/"
#define TEXTURES_FOLDER "Library/Textures/"
#define MATERIALS_FOLDER "Library/Materials/"

typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Thomas Engine"
#define ORGANIZATION_NAME "UPC CITM"