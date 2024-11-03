// Copyright Tim Rawcliffe / Rawcliffe Enterprises

#include "SDLApp.h"

// SDL
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

// ImGui
#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"

//______________________________________________________________________________________________________________________

// App Properties
/*static*/ std::string  SDLApp::AppNameStr  = std::string();
/*static*/ bool         SDLApp::HasInited   = false;
/*static*/ bool         SDLApp::AppRunning  = false;
/*static*/ SDLApp::SDLAppOptions SDLApp::Options = SDLApp::SDLAppOptions();

// SDL Stuff
/*static*/ SDL_Window*      SDLApp::Window = nullptr;
/*static*/ SDL_GLContext    SDLApp::GLContext = SDL_GLContext();

//______________________________________________________________________________________________________________________

namespace SDLAppImGui
{
    SDLAppImGuiOptions Options = SDLAppImGuiOptions();
    
    bool IsDrawingImGui = true;

    void InitImGui(const char* GLSLVersion, SDL_Window* Window, SDL_GLContext GLContext)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        
        io.ConfigDockingWithShift = true;
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
        
        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForOpenGL(Window, GLContext);
        ImGui_ImplOpenGL3_Init(GLSLVersion);
        
        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != nullptr);
    }

    void UninitImGui()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }

    bool DrawUpdate(ImGuiIO& io, std::string& AppNameStr, SDLApp::SDLAppOptions& AppOptions)
    {
        bool ContinueRunningApp = true;
        
        // Start the Dear ImGui frame
        ImGui::NewFrame();
        
        ImGui::DockSpaceOverViewport();
        
        if (Options.MainMenuBar)
        {
            ImGui::BeginMainMenuBar();
            
            if (ImGui::BeginMenu("App"))
            {
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::ColorEdit4("Screen Clear Color", &AppOptions.ScreenClearColor[0]);
                    
                    ImGui::EndMenu();
                }
                
                if (ImGui::MenuItem("Exit"))
                {
                    ContinueRunningApp = false;
                }
                
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("ImGui"))
            {
                if (ImGui::BeginMenu("Display Options"))
                {
                    ImGui::Checkbox("Display App Name", &Options.MainMenu_AppName);
                    ImGui::Checkbox("Display App FPS", &Options.MainMenu_FPS);
                    
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Demos"))
                {
                    ImGui::Checkbox("ImGui Demo", &Options.ShowDemoWindow_ImGui);
                    ImGui::Checkbox("ImPlot Demo", &Options.ShowDemoWindow_ImPlot);
                    
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenu();
            }
            
            if (Options.MainMenu_AppName)
            {
                const ImVec2 AppNameSize = ImGui::CalcTextSize(AppNameStr.c_str());
                
                ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - AppNameSize.x) / 2.0f, ImGui::GetWindowPos().y));
                
                ImGui::Text("%s", AppNameStr.c_str());
            }
            
            if (Options.MainMenu_FPS)
            {
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 80, ImGui::GetWindowPos().y));
                
                ImGui::Text("FPS: % .01f", io.Framerate);
            }
            
            ImGui::EndMainMenuBar();
        }
        
        if (Options.ShowDemoWindow_ImGui)
        {
            ImGui::ShowDemoWindow(&Options.ShowDemoWindow_ImGui);
        }

        if (Options.ShowDemoWindow_ImPlot)
        {
            ImPlot::ShowDemoWindow(&Options.ShowDemoWindow_ImPlot);
        }
        
        // Rendering
        ImGui::Render();
        
        return ContinueRunningApp;
    }
}

//______________________________________________________________________________________________________________________

/*static*/ void SDLApp::Init(const char* AppName)
{
    if (HasInited)
    {
        return;
    }
    
    AppNameStr = AppName;
    
    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return ;
    }
    
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    
    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    SDLApp::Window = SDL_CreateWindow(AppNameStr.c_str(), 1280, 720, window_flags);
    if (SDLApp::Window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowPosition(SDLApp::Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDLApp::GLContext = SDL_GL_CreateContext(SDLApp::Window);
    SDL_GL_MakeCurrent(SDLApp::Window, SDLApp::GLContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_ShowWindow(SDLApp::Window);
    
    SDL_Renderer* Renderer = nullptr;
    SDL_Surface* Surface = SDL_CreateSurface(640, 480, SDL_PIXELFORMAT_RGBA8888);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    
    // Setup Dear ImGui context
    SDLAppImGui::InitImGui(glsl_version, SDLApp::Window, SDLApp::GLContext);
    
    HasInited = true;
    
    printf("AppName: '%s'", AppNameStr.c_str());
}

//______________________________________________________________________________________________________________________

/*static*/ int SDLApp::ExecuteApp()
{
    return ExecuteApp([](float DeltaTime) {});
}

/*static*/ int SDLApp::ExecuteApp(std::function<void(float)> UpdateFunction)
{
    if (!HasInited)
    {
        printf("SDLApp::ExecuteApp - App was not inited. Call SDLApp::Init() before SDLApp::ExecuteApp(). Exiting");
        return -1;
    }
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    AppRunning = true;
    
    while (AppRunning)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if ((event.type == SDL_EVENT_QUIT) ||
                (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(SDLApp::Window)))
            {
                AppRunning = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.scancode == SDLAppImGui::Options.ToggleVisibilityKey)
                {
                    SDLAppImGui::IsDrawingImGui = !SDLAppImGui::IsDrawingImGui;
                }
                else if (event.key.scancode == Options.ExitAppKey)
                {
                    AppRunning = false;
                }
            }
        }
        if (SDL_GetWindowFlags(SDLApp::Window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }
        
        const float DeltaTime = (1000.0f / io.Framerate);
        
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        
        // Run Update function
        UpdateFunction(DeltaTime);
        
        if (SDLAppImGui::IsDrawingImGui)
        {
            AppRunning &= SDLAppImGui::DrawUpdate(io, AppNameStr, Options);
        }
        
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(Options.ScreenClearColor[0] * Options.ScreenClearColor[3], Options.ScreenClearColor[1] * Options.ScreenClearColor[3], Options.ScreenClearColor[2] * Options.ScreenClearColor[3], Options.ScreenClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);
        if (SDLAppImGui::IsDrawingImGui)
        {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                SDL_GL_MakeCurrent(SDLApp::Window , SDLApp::GLContext);
            }
        }
        else
        {
            SDL_GL_MakeCurrent(SDLApp::Window , SDLApp::GLContext);
        }
        
        SDL_GL_SwapWindow(SDLApp::Window);
    }
    
    SDLAppImGui::UninitImGui();
    
    SDL_GL_DestroyContext(SDLApp::GLContext);
    SDL_DestroyWindow(SDLApp::Window);
    SDL_Quit();
    
    return 0;
}

//______________________________________________________________________________________________________________________
