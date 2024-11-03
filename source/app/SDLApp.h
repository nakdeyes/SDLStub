// Copyright Tim Rawcliffe / Rawcliffe Enterprises

#pragma once

#include <functional>
#include <string>

#include <SDL3/SDL_video.h>

// forward declarations
class SDL_Window;

class SDLApp
{
public:
    static void Init(const char* AppName);
    
    static int ExecuteApp();
    static int ExecuteApp(std::function<void(float)> UpdateFunction);
    
    
    
private:
    // App properties
    static std::string AppNameStr;
    static bool HasInited;
    static bool AppRunning;
    
    // SDL Stuff
    static SDL_Window* Window;
    static SDL_GLContext GLContext;
};
