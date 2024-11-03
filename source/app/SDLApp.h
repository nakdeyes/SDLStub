// Copyright Tim Rawcliffe / Rawcliffe Enterprises

#pragma once

#include <functional>
#include <string>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

// forward declarations
class SDL_Window;

//______________________________________________________________________________________________________________________

namespace SDLAppImGui
{
    // Options struct for SDLApp ImGui functionality.
    struct SDLAppImGuiOptions
    {
        // If disalbed SDLApp will not try to draw any main menu bar. Else a main menu bar with various options for SDLApp
        //  ImGui will be drawn.
        bool MainMenuBar = true;
        
        // Draw the FPS in the main menu.
        bool MainMenu_FPS = true;
        
        // Draw the app name in the main menu.
        bool MainMenu_AppName = true;
        
        // Show the demo window for ImGui
        bool ShowDemoWindow_ImGui = false;
        
        // Show the demo window for ImPlot
        bool ShowDemoWindow_ImPlot = false;
        
        // SDL Scan code to toggle ImGui visibility with.
        SDL_Scancode ToggleVisibilityKey = SDL_SCANCODE_BACKSLASH;
    };
    
    // Global flag for if ImGui is drawing according to the app. Up to user to read flag, so you can still draw when this is off.
    extern bool IsDrawingImGui;

    // The static global instance of the options.
    extern SDLAppImGuiOptions Options;
}

//______________________________________________________________________________________________________________________

class SDLApp
{
public:
    static void Init(const char* AppName);
    
    static int ExecuteApp();
    static int ExecuteApp(std::function<void(float)> UpdateFunction);
    
    // Options struct for SDL App options.
    struct SDLAppOptions
    {
        // SDL Scan code to exit the App.
        SDL_Scancode ExitAppKey = SDL_SCANCODE_ESCAPE;
        
        // Screen clear color, the color that will be drawn if nothing else is. In RGBA format
        float ScreenClearColor[4] = {0.45f, 0.55f, 0.60f, 1.00f};
    };
    static SDLAppOptions Options;
    
private:
    // App properties
    static std::string AppNameStr;
    static bool HasInited;
    static bool AppRunning;
    
    // SDL Stuff
    static SDL_Window* Window;
    static SDL_GLContext GLContext;
};

//______________________________________________________________________________________________________________________
