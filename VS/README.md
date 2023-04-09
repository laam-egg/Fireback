# Note on Visual Studio project files

## IDE  
To use these files you must have Visual Studio installed on your local machine.  
This project is checked to compile fine with Visual Studio 2022.

## Setup third-party libraries  
To set up SDL2 and its plugins, you first have to install [vcpkg](https://vcpkg.io/en/getting-started.html).  
Then, open the terminal and type:
> vcpkg install sdl2:x64-windows  
> vcpkg install sdl2-image:x64-windows  
> vcpkg install sdl2-ttf:x64-windows  
> vcpkg install sdl2-mixer:x64-windows  
> vcpkg integrate install  

## Compile and run the program  
1. Open the file Fireback.sln first.
2. Select desired configuration (Debug or Release).
3. Build and/or Debug the program.
