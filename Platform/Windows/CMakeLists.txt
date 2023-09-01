add_library(WindowsApplication WindowsApplication.cpp)
add_executable(MyGameEngine  D3d12Application.cpp)
target_link_libraries(MyGameEngine WindowsApplication Common D3dRHI d3d12 dxgi d3dcompiler)

add_executable(MyGameEngineOpenGL 
    OpenGLApplication.cpp
    ${PROJECT_SOURCE_DIR}/External/GL/src/glad_wgl.c
    )
target_link_libraries(MyGameEngineOpenGL WindowsApplication Common OpenGLRHI opengl32)
target_include_directories(MyGameEngineOpenGL PRIVATE "${PROJECT_SOURCE_DIR}/External/GL/include")

add_subdirectory(test)