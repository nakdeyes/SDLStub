// Copyright Tim Rawcliffe / Rawcliffe Enterprises

#include <string>

#include <SDL3/SDL_video.h>

// forward declarations
class SDL_Window;

class SDLApp
{
public:
    static void Init(const char* AppName);
    
    static int ExecuteApp();
    
private:
    // App properties
    static std::string AppNameStr;
    static bool HasInited;
    static bool AppRunning;
    
    // SDL Stuff
    static SDL_Window* Window;
    static SDL_GLContext GLContext;
};
