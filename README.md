## SDL3 Sample app w/ ImGui implemented for quick SDL / ImGui app creation
Started with the official SDL3 Sample, then stripped it down a bit and implemented ImGui on top for easy UI creation. CMake is a requirement as we use CMake FetchContent to pull down SDL3 and Imgui libraries down at build time. ImGui implmentation is using the Docking branch. ImPlot will likely come in soon as well. 

### Building And Running
Clone the depot, ensure CMake is installed, and then run one of the platform specific setup scripts in the config directory depending on your preferred development platform.

Open the generated solution files in your IDE, compile, and enjoy!

## Supported Platforms
The underlying SDL Sample should support the following platforms, however the Emscripten platforms have not been directly tested on this project.
- macOS
- iOS
- tvOS
- visionOS
- Windows
- Linux
- Web browsers (via Emscripten)*
- Android*

*See further instructions in [`config/`](config/)

Note: UWP support was [removed from SDL3](https://github.com/libsdl-org/SDL/pull/10731) during its development. For historical reasons, you can a working UWP sample via this commit: [df270da](https://github.com/Ravbug/sdl3-sample/tree/df270daa8d6d48426e128e50c73357dfdf89afbf)

## Updating SDL / ImGui / 3rdParties
Currently, the commits at which to sync these dependencies are in the root CMakeLists.txt. You can also likely do a 'git pull' in their 3rdParties directories after initial project generation.
