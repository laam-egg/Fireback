# Note on CodeBlocks project files

## IDE
To use these files you must have Code::Blocks installed on your machine.  
This project is checked to compile fine with Code::Blocks 20.03 (GCC MinGW-w64 8.1.0).  

## Setup third-party libraries
To set up SDL2 and its plugin, you first have to download the following packages:  
 - SDL2 package named ``SDL2-devel-X.Y.Z-mingw.zip`` from <https://github.com/libsdl-org/SDL/releases/latest> ;  
 - SDL2_image package named ``SDL2_image-devel-X.Y.Z-mingw.zip`` from <https://github.com/libsdl-org/SDL_image/releases/latest> ;  
 - SDL2_ttf package named ``SDL2_ttf-devel-X.Y.Z-mingw.zip`` from <https://github.com/libsdl-org/SDL_ttf/releases/latest> ;  
 - SDL2_mixer package named ``SDL2_mixer-devel-X.Y.Z-mingw.zip`` from <https://github.com/libsdl-org/SDL_mixer/releases/latest>.  

Extract each package to individual folders. Suppose their folders are respectively ``{SDL2_ROOT}``, ``{SDL2_image_ROOT}``, ``{SDL2_ttf_ROOT}``, ``{SDL2_mixer_ROOT}``. *(substitute those symbols with the corresponding real paths)*.  

Now open Code::Blocks and navigate to menu Settings->Compiler. Switch to "Search directories" tab and add the following paths to the Compiler box:

```
{SDL2_ROOT}\x86_64-w64-mingw32\include
{SDL2_ROOT}\x86_64-w64-mingw32\include\SDL2
{SDL2_image_ROOT}\x86_64-w64-mingw32\include
{SDL2_ttf_ROOT}\x86_64-w64-mingw32\include
{SDL2_mixer_ROOT}\x86_64-w64-mingw32\include
```

Hit OK to save settings. At this point, third-party library configuration for Code::Blocks has completed.

## Compile and run the program
1. Open the file Fireback.cbp with Code::Blocks first.
2. Select desired configuration (Debug or Release).
3. Build and/or Debug the program as usual.
