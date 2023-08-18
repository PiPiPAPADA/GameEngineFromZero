# Compile cmd
all compile cmd

## d3d12 compile cmd
cl /EHsc helloengine_d3d12.cpp user32.lib d3d12.lib dxgi.lib d3dcompiler.lib

## d3d12 torus build
cl /EHsc -I./DirectXMath helloengine_d3d12.cpp user32.lib d3d12.lib d3dcompiler.lib dxgi.lib