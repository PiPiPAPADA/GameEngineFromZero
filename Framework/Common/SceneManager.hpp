#pragma once
#include "geommath.hpp"
#include "IRuntimeModule.hpp"
#include "SceneParser.hpp"

namespace My
{
    class SceneManager : implements IRuntimeModule
    {    
    public:
        virtual ~SceneManager();
        virtual int Initialize();
        virtual void Finalize();
        virtual void Tick();
        bool LoadOgexScene(const char* scene_file_name);
        int LoadScene(const char* scene_file_name);

        const Scene& GetSceneForRendering();

    protected:
       std::unique_ptr<Scene> m_pScene;
    };
    
    extern SceneManager* g_pSceneManager;
    
    
} // namespace My
