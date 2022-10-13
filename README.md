# GameEngine

Making a game engine "with no libraries", following handmade hero videos.

I'm currently using c/c++ and Opengl for rendering.

Features:
  - Win32 Window
  - Using Modern context opengl 3.3 Core
  - Loading opengl function pointers
  - Batch Rendering
  - Font rendering using stb_truetype
  - Obj Loading and rendering
  - Very simple particle system
  - Simple Math library for opengl
  - Memory management using memory arenas

Todo:
  - Skeletal animation (md5)
  - Physically based rendering
  - Shadows & Lighting
  - Add Linux Support Window
  - Create texture atlas for font rendering(now using texture arrays)
  - Push static procedural meshes


Dependencies:
  - STB image
  - STB True Type
  
## Compile
# Windows
run ./build.bat
# Linux
run ./build.sh
