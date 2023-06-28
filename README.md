# GraphicsEngine
[![Lines of code](https://tokei.rs/b1/github/Szczurox/GraphicsEngine)](https://github.com/Szczurox/GraphicsEngine)
[![License](https://img.shields.io/github/license/Szczurox/GraphicsEngine?service=github)](https://github.com/Szczurox/GraphicsEngine/blob/main/LICENSE)   
Simple Win32 graphics engine built from scratch.   
Uses Win32 for Windows 2000 or higher, doesn't work on older versions.   

## Getting started
Firstly, make sure that you use Windows as Linker's subsystem if you are using Visual Studio as your project's IDE.   
You can do it by going to `Project > Properties > Linker > System > SubSystem`.   
After doing that go check some [demo projects](https://github.com/Szczurox/GraphicsEngine/tree/main/src/demo), they should give you a good grasp of functionalities this engine offers.   
To run them u need to [include their header file in a cpp file and run demo's main function inside WinMain](https://github.com/Szczurox/GraphicsEngine/blob/main/src/main.cpp).   
