To get Visual Studio Project setup:
-Start a new Win32 Empty C++ Project
-Add Linkers: d3d11.lib;d3dx11d.lib;D3DCompiler.lib;Effects11.lib;
-Add DirectxSDK/include and DirectXSDK/lib/x86 directories
-Add MultiFall/include and MultiFall/lib directories
-Add all files in MultiFall/src to project and build
-Also, for now, you need to copy the content folder to the Visual Studio project directory